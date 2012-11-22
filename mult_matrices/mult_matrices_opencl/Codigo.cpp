#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/cl.h>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include <SDKCommandArgs.hpp>
#include <SDKFile.hpp>


const char *programSource =
"__kernel                                       \n"
"void matrixMul(__global int *C,                \n"
"          const int hA,                        \n"
"          const int wA,                        \n"
"          const int hB,                        \n"
"          const int wB,                        \n"
"          __global int *A,                     \n"
"          __global int *B)                     \n"
"{                                              \n"
"                                               \n"
"  int row=get_global_id(0);                    \n"
"  int col=get_global_id(1);                    \n"
"  int sum;                                     \n"
"  int i;                                       \n"
"  sum=0;                                       \n"
"  for(i=0; i<wA; i++)                          \n"
"    sum+=A[row*wA+i]*B[i*wB+col];              \n"
"  C[row*wB+col]=sum;                           \n"
"}                                              \n"
;

int main() {
    unsigned int i,j;
    size_t hA,wA,hB,wB,wC,hC;
    long int *A=NULL;
    long int *B=NULL;
    long int *C=NULL;
    printf("Size of long int: %d\n", sizeof(long int));
    printf("Size of OpenCL long: %d\n", sizeof(cl_long));
    time_t start,end;
    hA=128;
    wA=128;
    hB=128;
    wB=128;
    hC=hA;
    wC=wB;

    //C=A*B
    A=(long int *)malloc(sizeof(long int)*hA*wA);
    B=(long int *)malloc(sizeof(long int)*hB*wB);
    C=(long int *)malloc(sizeof(long int)*hC*wC);

    for (i=0;i<hA;i++)
        for (j=0;j<wA;j++) { *(A+wA*i+j)=1; }
        //A[i][j]=1;

    for (i=0;i<hB;i++)
        for (j=0;j<wB;j++) { *(B+wB*i+j)=1; }
        //B[j][i]=1;

    for (i=0;i<hC;i++)
        for (j=0;j<wC;j++) { *(C+wC*i+j)=0; }

    cl_int status;

    //PASO 1: Identificar e inicializar las plataformas
    cl_uint numPlatforms=0;
    cl_platform_id *platforms=NULL;
    status=clGetPlatformIDs(0,NULL,&numPlatforms);
    platforms=(cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));
    status=clGetPlatformIDs(numPlatforms,platforms,NULL);
    printf("number of platforms: %d\n", (int)numPlatforms);

    //PASO 2: Identificar e inicializar los dispositivos
    cl_uint numDevices=0;
    cl_device_id *devices=NULL;    
    status=clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,0,NULL,&numDevices);
    devices=(cl_device_id*)malloc(numDevices*sizeof(cl_device_id));    
    status=clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,numDevices,devices,NULL);    
    printf("number of devices: %ld\n", (long int)numDevices);

    //PASO 3: Crear el contexto
    cl_context context = NULL;
    context=clCreateContext(0,numDevices,devices,NULL,NULL,&status);

    //PASO 4: Crear la fila de ordenes ("command queue")
    cl_command_queue cmdQueue;
    cmdQueue=clCreateCommandQueue(context,devices[0],0,&status);

    //PASO 5: Crear los "device buffers"
    cl_mem bufferA;
    cl_mem bufferB;
    cl_mem bufferC;
    bufferA=clCreateBuffer(context,CL_MEM_READ_ONLY,sizeof(long int)*hA*wA,NULL,&status);
    bufferB=clCreateBuffer(context,CL_MEM_READ_ONLY,sizeof(long int)*hB*wB,NULL,&status);
    bufferC=clCreateBuffer(context,CL_MEM_WRITE_ONLY,sizeof(long int)*hC*wC,NULL,&status);

    //PASO 6: Pasar los datos de la memoria del CPU a los "buffers" del dispositivo
    status=clEnqueueWriteBuffer(cmdQueue,bufferA,CL_FALSE,0,sizeof(long int)*hA*wA,A,0,NULL,NULL);
    status=clEnqueueWriteBuffer(cmdQueue,bufferB,CL_FALSE,0,sizeof(long int)*hB*wB,B,0,NULL,NULL);

    //PASO 7: Crear y compilar el programa
    cl_program program=clCreateProgramWithSource(context,1,(const char**)&programSource,NULL,&status);
    status=clBuildProgram(program,numDevices,devices,NULL,NULL,NULL);

    switch(status) {
        case CL_INVALID_PROGRAM:
                                printf("CL_INVALID_PROGRAM\n");
                                break;
        case CL_INVALID_VALUE:
                                printf("CL_INVALID_VALUE\n");
                                break;
        case CL_INVALID_DEVICE:
                                printf("CL_INVALID_DEVICE\n");
                                break;
        case CL_INVALID_BINARY:
                                printf("CL_INVALID_BINARY\n");
                                break;
        case CL_INVALID_BUILD_OPTIONS:
                                printf("CL_INVALID_BUILD_OPTIONS\n");
                                break;
        case CL_INVALID_OPERATION:
                                printf("CL_INVALID_OPERATION\n");
                                break;
        case CL_COMPILER_NOT_AVAILABLE:
                                printf("CL_COMPILER_NOT_AVAILABLE\n");
                                break;
        case CL_BUILD_PROGRAM_FAILURE:
                                printf("CL_BUILD_PROGRAM_FAILURE\n");
                                break;
        case CL_OUT_OF_HOST_MEMORY:
                                printf("CL_OUT_OF_HOST_MEMORY\n");
                                break;
        default:
                                if (status!=0)
                                    printf("Wierd build error\n");
                                break;
    }

    //PASO 8: Crear el kernel
    cl_kernel kernel=NULL;
    kernel=clCreateKernel(program,"matrixMul",&status);

    //PASO 9: Pasar argumentos al kernel
    status=clSetKernelArg(kernel,0,sizeof(cl_mem),&bufferC);
    status|=clSetKernelArg(kernel,1,sizeof(cl_int),&hA);
    status|=clSetKernelArg(kernel,2,sizeof(cl_int),&wA);
    status|=clSetKernelArg(kernel,3,sizeof(cl_int),&hB);
    status|=clSetKernelArg(kernel,4,sizeof(cl_int),&wB);
    status|=clSetKernelArg(kernel,5,sizeof(cl_mem),&bufferA);
    status|=clSetKernelArg(kernel,6,sizeof(cl_mem),&bufferB);

    //Paso 10: Configurar la estructura del "work-item"
    size_t localws[2]={16,16};
    size_t globalws[2]={wC,hC};

    //PASO 11: Enfilar el "kernel" para ejecucion
    start=clock();
    status=clEnqueueNDRangeKernel(cmdQueue,kernel,2,NULL,globalws,localws,0,NULL,NULL);
    clFinish(cmdQueue);
    end=clock();

    //PASO 12: Transportar el "buffer" de resultado a la memoria del CPU
    clEnqueueReadBuffer(cmdQueue,bufferC,CL_TRUE,0,hC*wC*sizeof(long int),C,0,NULL,NULL);

    for (i=0;i<hC;i++) {
        printf("C[%03d]",i);
        for (j=0;j<wC;j++) {
            if(j < 16 ) {
                printf(" %li",*(C+wC*i+j));
            } else if(j == 16) {
                printf("...");
            }
        }

        printf("\n");
    }

    printf("Tiempo: %ld ms\n",end-start);

    //PASO 13: Liberar recursos OpenCL
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseContext(context);

    free(A);
    free(B);
    free(C);
    free(platforms);
    free(devices);

    return 0;
}