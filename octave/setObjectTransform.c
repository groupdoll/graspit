#include "connectToServer.h"
#include <stdio.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#endif

#define MAXLENGTH 500

#include "mex.h"

void mexFunction(int nlhs,       mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
  char buf[MAXLENGTH];
  char numStr[20];

  double value;
  double *pind,*poutvals;
  const double *pvals;
  int i,j,numBodies,*numDOF;
  int sockd;
  const mxArray *vals;
  mxArray *outvals;

  /* Check for proper number of arguments */
  
  if (nrhs < 1) {
    mexErrMsgTxt("setObjectTransform takes at least one input argument.");
  } 
  if (nrhs > 2) {
	mexErrMsgTxt("setObjectTransform takes at most two input arguments.");
  }
  if (nlhs > 0) {
		mexErrMsgTxt("setObjectTransform takes no output argument.");
	}

//  if (nlhs > 1) {
//    mexErrMsgTxt("setObjectTransform takes at most one output argument.");
//  }
//  if (nlhs == 0 ) nlhs=1;
	
  // more argument checking is needed 

  sockd = ConnectTo("localhost",4765);

  if (sockd < 0)
    mexErrMsgTxt("Could not connect");
 
  sprintf(buf,"setObjectTransform ");

  // if no bodies were specified by user, read total number of bodies
  if (nrhs<2) {
	strcat(buf,"ALL ");
	if (mxIsCell(prhs[0]))
	  numBodies = mxGetNumberOfElements(prhs[0]);
	else numBodies = 1;
	  
  }
  // otherwise send the body list
  else {
	numBodies = mxGetNumberOfElements(prhs[1]);
	if (numBodies>0) {
	  sprintf(numStr,"%d ",numBodies);
	  strcat(buf,numStr);

	  pind = mxGetPr(prhs[1]);
	  for (i=0;i<numBodies;i++) {
	    sprintf(numStr,"%d ",(int)pind[i]-1);
  		strcat(buf,numStr);
	  }
	}
  }

  if (numBodies > 1) {
	if (!mxIsCell(prhs[0]))
	  mexErrMsgTxt("Argument 1 should be a cell array.");
	if (mxGetNumberOfElements(prhs[0]) != numBodies)
	  mexErrMsgTxt("Argument 1 should have a cell for each body.");
  }

  if (numBodies == 0) {
		plhs[0] = NULL;
  }
  else {
		numDOF = (int *)mxCalloc(numBodies,sizeof(int));

    for (i=0;i<numBodies;i++) {
			if (numBodies>1)
				vals = mxGetCell(prhs[0],i);
			else
				vals = prhs[0];

			pvals = mxGetPr(vals);
			
			for (j=0;j<7;j++) {
				sprintf(numStr,"%lf ",pvals[j]);
				strcat(buf,numStr);
			}
		}

	strcat(buf,"\n");
	Writeline(sockd,buf,strlen(buf));

//	if (numBodies > 1)
//      plhs[0] = mxCreateCellArray(1,&numBodies);
//
//	for (i=0;i<numBodies;i++) {
//	  outvals = mxCreateDoubleMatrix(numDOF[i],1,mxREAL);
//	  poutvals = mxGetPr(outvals);
//
//	  for (j=0;j<numDOF[i];j++) {
//        Readline(sockd,buf,MAXLENGTH);
//		if (!strncmp(buf,"Error",5)) {
//		  mxFree(numDOF);
//		  mexErrMsgTxt(buf);
//		}
//        sscanf(buf,"%lf\n",&value);
//        poutvals[j] = value;
//	  }
//	  
//	  if (numBodies == 1) {
//		plhs[0] = outvals;
//	  } else {
//	    mxSetCell(plhs[0],i,outvals);
//	  }
//	}
		mxFree(numDOF);
  }

  CloseConnection(sockd);
  return;
}