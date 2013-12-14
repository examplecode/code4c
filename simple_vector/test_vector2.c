/*
 * =====================================================================================
 *
 *       Filename:  test_vector.c
 *
 *    Description:  the sample vector test
 *
 *        Version:  1.0
 *        Created:  2013/12/10 17时07分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include <assert.h>
#include <stdio.h>

typedef struct _student
{
	char name[50];
	int age;

} student;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
	int
main ( int argc, char *argv[] )
{
	vector * vec = (vector *) malloc(sizeof(vector));
	vector_init(vec);

	int i;
	for(i = 0 ; i < 100 ; i++ )
	{
		student * stu = (student*) malloc(sizeof(student));

		sprintf(stu->name,"Stu %d",i);
		vector_add(vec,stu);
	}


	vector_delete(vec,2);

	vector_delete(vec,30);

	int count = vector_count(vec);
	for(i= 0; i< count; i ++) 
	{
		student * stu = vector_get(vec,i);

		printf("Stu:%s\n",stu->name);
	}


	for(i= 0; i< vector_count(vec); i ++) 
	{
		student * stu = vector_get(vec,i);
		printf("Stu:%s\n",stu->name);
		vector_delete(vec,i);
		free(stu);
	}

	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
