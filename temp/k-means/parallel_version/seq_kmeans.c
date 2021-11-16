/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   File:         seq_kmeans.c  (sequential version)                        */
/*   Description:  Implementation of simple k-means clustering algorithm     */
/*                 This program takes an array of N data objects, each with  */
/*                 M coordinates and performs a k-means clustering given a   */
/*                 user-provided value of the number of clusters (K). The    */
/*                 clustering results are saved in 2 arrays:                 */
/*                 1. a returned array of size [K][N] indicating the center  */
/*                    coordinates of K clusters                              */
/*                 2. membership[N] stores the cluster center ids, each      */
/*                    corresponding to the cluster a data object is assigned */
/*                                                                           */
/*   Author:  Wei-keng Liao                                                  */
/*            ECE Department, Northwestern University                        */
/*            email: wkliao@ece.northwestern.edu                             */
/*                                                                           */
/*   Copyright (C) 2005, Northwestern University                             */
/*   See COPYRIGHT notice in top-level directory.                            */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>

#include "kmeans.h"


/*----< euclid_dist_2() >----------------------------------------------------*/
/* square of Euclid distance between two multi-dimensional points            */
__inline static
float euclid_dist_2(int    numdims,  /* no. dimensions */
                    float *coord1,   /* [numdims] */
                    float *coord2)   /* [numdims] */
{
    int i;
    float ans=0.0;
    
    for (i=0; i<numdims; i++){
        ans += (coord1[i]-coord2[i]) * (coord1[i]-coord2[i]);
    }
    return(ans);
}

/*----< find_nearest_cluster() >---------------------------------------------*/
__inline static
int find_nearest_cluster(int     numClusters, /* no. clusters */
                         int     numCoords,   /* no. coordinates */
                         float  *object,      /* [numCoords] */
                         float **clusters)    /* [numClusters][numCoords] */
{
    int   index, i;
    float dist, min_dist;

    /* find the cluster id that has min distance to object */
    index    = 0;
    min_dist = euclid_dist_2(numCoords, object, clusters[0]);

    for (i=1; i<numClusters; i++) {
        dist = euclid_dist_2(numCoords, object, clusters[i]);
        /* no need square root */
        if (dist < min_dist) { /* find the min and its array index */
            min_dist = dist;
            index    = i;
        }
    }

    return(index);
}

/*----< seq_kmeans() >-------------------------------------------------------*/
/* return an array of cluster centers of size [numClusters][numCoords]       */
int seq_kmeans(float **objects,      /* in: [numObjs][numCoords] */
               int     numCoords,    /* no. features */
               int     numObjs,      /* no. objects */
               int     numClusters,  /* no. clusters */
               float   threshold,    /* % objects change membership */
               int    *membership,   /* out: [numObjs] */
               float **clusters)     /* out: [numClusters][numCoords] */

{
    int      i, j, index, loop=0;
    int     **newClusterSize; /* [numClusters]: no. objects assigned in each
                                new cluster */
    float    delta;          /* % of objects change their clusters */
    float  ***newClusters;    /* [numClusters][numCoords] */
    int total_threads = omp_get_max_threads();
    int current_thread;



    /* initialize membership[] */
    #pragma omp parallel for schedule(static, 100)
    for (i=0; i<numObjs; i++)
        membership[i] = -1;

    /* need to initialize newClusterSize and newClusters[0] to all 0 */    
    newClusterSize = (int**) malloc(total_threads * sizeof(int*));
    assert(newClusterSize != NULL);
    newClusterSize[0] = (int *) calloc(total_threads * numClusters, sizeof(int));
    for (i=1; i<total_threads; i++)
        newClusterSize[i] = newClusterSize[i-1] + numClusters; //2D array



    newClusters    = (float ***) malloc(total_threads *            sizeof(float **));
    assert(newClusters != NULL);
    newClusters[0] = (float **)  malloc(numClusters * total_threads * sizeof(float *));
    assert(newClusters[0] != NULL);

    for (i=1; i<total_threads; i++)
            newClusters[i] = newClusters[i-1] + numClusters;
        for (i=0; i<total_threads; i++) {
            for (j=0; j<numClusters; j++) {
                newClusters[i][j] = (float*)calloc(numCoords, sizeof(float));
                assert(newClusters[i][j] != NULL);
            }
        }

    do {
        delta = 0.0;
        #pragma omp parallel 
        {

            #pragma omp for reduction(+:delta) private(index, current_thread) 
            for (i=0; i<numObjs; i++) {
                current_thread = omp_get_thread_num();
                /* find the array index of nestest cluster center */
                index = find_nearest_cluster(numClusters, numCoords, objects[i],
                                            clusters);

                /* if membership changes, increase delta by 1 */
                if (membership[i] != index) 
                    delta += 1.0;

                /* assign the membership to object i */
                membership[i] = index;
                
                /* update new cluster center : sum of objects located within */
                newClusterSize[current_thread][index]++;
                for (j=0; j<numCoords; j++)
                    newClusters[current_thread][index][j] += objects[i][j];
            }
        
            /* average the sum and replace old cluster center with newClusters */
            #pragma omp for private (current_thread)
            for (i=0; i<numClusters; i++) {
                current_thread = omp_get_thread_num();
                for (j=0; j<numCoords; j++) {
                    if (newClusterSize[current_thread][i] > 0)
                        clusters[i][j] = newClusters[current_thread][i][j] / newClusterSize[current_thread][i];
                    newClusters[current_thread][i][j] = 0.0;   /* set back to 0 */
                }
                newClusterSize[current_thread][i] = 0;   /* set back to 0 */
            }
        }
        
        delta /= numObjs;
    } while (delta > threshold && loop++ < 500);

    free(newClusters[0]);
    free(newClusters);
    free(newClusterSize);
            
    return 1;
}
