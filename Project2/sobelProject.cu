#include <stdio.h>
#include <iostream>
#include <ctime>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// Waris     Vorathumdusadee Section 3 6088128
// Sirichoke Yooyen          Section 3 6088232
// Phummarat Yosamornsuntorn Section 3 608823

// this is the kernel for change RGB to grey color
__global__ void ChangeToGrey(unsigned char *rgb, unsigned char *grey, int Row, int Column) {

    //find the global ID of each row
    int ColIDThread = threadIdx.x + blockIdx.x * blockDim.x;
    //find the global ID of each columns
    int RowIDThread = threadIdx.y + blockIdx.y * blockDim.y;


          int NumberOffset = RowIDThread * Column + ColIDThread;
          //multiply 3 because 3 color which is r g b
          int rgb_offset = NumberOffset * 3;
          //
          float weight[3] = {0.22,0.72,0.07};
          // this is the weight that to compute the gray scale
          float sum=0; //sum for compute the value that of all RGB color

          //this loop for find the value of Gray color
          for(int i=0;i<3 ;i++)
          {
            sum += rgb[rgb_offset+i]*weight[i];
          }
          grey[NumberOffset] = sum;

}
// this is the kernel for find the sobel of X and Y
__global__ void ChangeToSobel(unsigned char *grayData,
  unsigned char *ValueSobelX,
  unsigned char *ValueSobelY,
  int Row,
  int Column) {
    //find the global ID of each columns
    int ColIDThread = threadIdx.x + blockIdx.x * blockDim.x;
    //find the global ID of each row
    int RowIDThread = threadIdx.y + blockIdx.y * blockDim.y;
        // calculate Global threadID
        int offset = RowIDThread * Column + ColIDThread;
        int ValueOfMargin[3][3];  //it is used for save the value of margin of that pixel
        // set the matrix value for find the sobel of horizontal
        int Horizon[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};
        // set the matrix value for find the sobel of vertical
        int Vertic[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};

        int step = -1; //step for find the neighbor
        // this loop for find the neighbor of that pixel
        for(int i=0;i<3;i++)
        {
          for(int j=0;j<3;j++)
          {

              ValueOfMargin[i][j] = (RowIDThread+(i-1))*Column + (ColIDThread+step);
              //set the value of margin of that pixel
              step++;
              // increase because use for go to the pixel
          }
          step=-1;
        }
        int sumHorizon = 0;// set the matrix value for find the sobel of horizontal
        int sumVertical = 0;// set the matrix value for find the sobel of vertical
        // this loop for find the sobel in the vertical and horizontal
        for(int i = 0; i < 3; i ++){
            for(int j = 0; j < 3; j++){

                int TempX = sumHorizon;
                // TempX for save the previous value of horizontal
                int TempY = sumVertical;
                // TempY for save the previous value of vertical
                sumVertical = (grayData[ValueOfMargin[i][j]] * Horizon[i][j]);
                sumVertical=sumVertical+TempY;
                // set the new value sum of vertical
                sumHorizon = (grayData[ValueOfMargin[i][j]] * Vertic[i][j]);
                sumHorizon = TempX+sumHorizon;
                  // set the new value sum of horizontal
            }

        }
          // set the value sum of horizontal if it more than 255 set it to 255
          // because the value cannot exceed 255
        if(sumHorizon >= 255){
            sumHorizon = 255;
        }
        // set the value sum of horizontal if it less than 0 set it to 0
        // because the value cannot less than 0
        if(sumHorizon <= 0){
            sumHorizon = 0;
        }
        // set the value sum of vertical if it more than 255 set it to 255
        // because the value cannot exceed 255
        if(sumVertical >= 255){
            sumVertical = 255;
        }
        // set the value sum of vertical if it less than 0 set it to 0
        // because the value cannot less than 0
        if(sumVertical <= 0){
            sumVertical = 0;
        }
        //set the value of sobelX
        ValueSobelX[offset] = sumHorizon;
        //set the value of sobelY
        ValueSobelY[offset] = sumVertical;

}

int main(int argc, char **argv) {

    clock_t begin = clock();
	  std::string input_file;
    std::string OutputFilename[3];
    // OutputFilename[0] for gray image output
    // OutputFilename[1] for sobelX of image output
    // OutputFilename[2] for sobelY of image output

	//Check for the input file that user input or not
      if(argc==1)
    {
      std::cerr << "Please input your image file in the directory first.\n";
        std::cerr << "And type ./sobelProject filename.jpg\n";
      exit(1);
    }
    // set the value of output file use for write the output to the user
			      input_file = std::string(argv[1]);
            OutputFilename[0] = "Gray_Of_"+std::string(argv[1]);
            OutputFilename[1] = "OutSobelX_Of_"+std::string(argv[1]);
            OutputFilename[2] = "OutSobelY_Of_"+std::string(argv[1]);
    unsigned char *DataForCopy[4];
    // That use for cuda to do in the device
    // DataForCopy[0] for array for storing rgb data
    //DataForCopy[1] for data of grey
    //DataForCopy[2] for data of sobelX
    //DataForCopy[3] for data of sobelY

    unsigned char *Data[3];
      // That use for host
    //Data[0] for data of grey
    //Data[1] for data of sobelX
    //Data[2] for data of sobelY


  	int RowOfPix; //number of rows of pixels
  	int ColumnfPix; //number of columns of pixels

    //read image data into DataImg Mat object
    cv::Mat DataImg = cv::imread(input_file, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
    if (DataImg.empty()) {
        std::cerr << "Please check your " << input_file << std::endl;
        std::cerr << "It is not in the same directory of .cu "<< std::endl;
    }
    // set the value of RowOfPix equal the row of image
    RowOfPix = DataImg.rows;
    // set the value of ColumnfPix equal the colums of image
    ColumnfPix = DataImg.cols;

    // find the total size of image
    const size_t SizeofAllPixel = DataImg.rows * DataImg.cols;
    // find the value of all pixel that contains RGB color
    const size_t SizeofAllRGB = 3*SizeofAllPixel;

	//this loop for allocate the memory
    for(int i=0;i<4;i++)
    {
      if(i==0)
      {
        //allocate for
        //DataForCopy[0] for array for storing rgb data
         cudaMalloc(&DataForCopy[i], sizeof(unsigned char)* SizeofAllRGB);
      }
      else
      {
         //allocate for
         //DataForCopy[1] for data of greydata
         //DataForCopy[2] for data of sobelX
         //DataForCopy[3] for data of sobelY
         //Data[0] for data of grey
         //Data[1] for data of sobelX
         //Data[2] for data of sobelY

        Data[i-1] = (unsigned char *)malloc(sizeof(unsigned char*)* SizeofAllPixel);
         cudaMalloc(&DataForCopy[i], sizeof(unsigned char) * SizeofAllPixel);
      }
    }
  cudaMemset(DataForCopy[1], 0, sizeof(unsigned char) * SizeofAllPixel);

	//use cudaMemcpy for copy data from host to device
    cudaMemcpy(DataForCopy[0], (unsigned char*)DataImg.data, sizeof(unsigned char) *SizeofAllRGB, cudaMemcpyHostToDevice);
    cudaMemcpy(DataForCopy[2], Data[1], sizeof(unsigned char) * SizeofAllPixel, cudaMemcpyHostToDevice);
    cudaMemcpy(DataForCopy[3], Data[2], sizeof(unsigned char) * SizeofAllPixel, cudaMemcpyHostToDevice);


  int Thread = 16; //maximum thread
	const dim3 dimBlock(Thread, Thread); //16*16 thread per blocks
  int x=ColumnfPix%Thread;
  int y=RowOfPix%Thread;
  // below condition for n divide by number of thread and remainder equal 0 or not
  if(x==0 && y==0)
  {

    	//this the cuda kernal and we set the dimGrid and dimBlock
      const dim3 dimGrid((ColumnfPix)/Thread, (RowOfPix)/Thread); //number of block
      ChangeToGrey<<<dimGrid, dimBlock>>>(DataForCopy[0], DataForCopy[1], RowOfPix, ColumnfPix);
      ChangeToSobel<<<dimGrid, dimBlock>>>(DataForCopy[1],DataForCopy[2], DataForCopy[3], RowOfPix, ColumnfPix);
  }
  if(x!=0 && y!=0)
  {

    	//this the cuda kernal and we set the dimGrid and dimBlock
      const dim3 dimGrid((int)ceil((ColumnfPix+Thread-1)/Thread), (RowOfPix+Thread-1)/Thread);//number of block
      ChangeToGrey<<<dimGrid, dimBlock>>>(DataForCopy[0], DataForCopy[1], RowOfPix, ColumnfPix);
      ChangeToSobel<<<dimGrid, dimBlock>>>(DataForCopy[1],DataForCopy[2], DataForCopy[3], RowOfPix, ColumnfPix);
  }
  if(x!=0 && y==0)
  {

    	//this the cuda kernal and we set the dimGrid and dimBlock
      const dim3 dimGrid((ColumnfPix+Thread-1)/Thread, (RowOfPix)/Thread);//number of block
      ChangeToGrey<<<dimGrid, dimBlock>>>(DataForCopy[0], DataForCopy[1], RowOfPix, ColumnfPix);
      ChangeToSobel<<<dimGrid, dimBlock>>>(DataForCopy[1],DataForCopy[2], DataForCopy[3], RowOfPix, ColumnfPix);
  }
  if(x!=0 && y!=0)
  {

    	//this the cuda kernal and we set the dimGrid and dimBlock
      const dim3 dimGrid((ColumnfPix)/Thread, (RowOfPix+Thread-1)/Thread);//number of block
      ChangeToGrey<<<dimGrid, dimBlock>>>(DataForCopy[0], DataForCopy[1], RowOfPix, ColumnfPix);
      ChangeToSobel<<<dimGrid, dimBlock>>>(DataForCopy[1],DataForCopy[2], DataForCopy[3], RowOfPix, ColumnfPix);
  }




	//copy computed gray data array from device to host
  for(int i=0;i<3;i++)
  {
    	//copy DataForCopy to Data by using cudaMemcpyDeviceToHost
      cudaMemcpy(Data[i], DataForCopy[i+1], sizeof(unsigned char) * SizeofAllPixel, cudaMemcpyDeviceToHost);
      	//set the Mat to save the value of data
        cv::Mat outputData(RowOfPix, ColumnfPix, CV_8UC1,(void *) Data[i]);
        // write the file by using above Mat
          cv::imwrite(OutputFilename[i].c_str(), outputData);
          // print to the user that it write file already
          std::cout <<"Write file " << OutputFilename[i] << std::endl;
  }
  // this loop for clear memory in that store in the array
  for(int i=0;i<4;i++)
  {
     cudaFree(DataForCopy[i]);
  }
  clock_t finish = clock();
  double timeuse = double(finish - begin);
  fflush(stdout);
  //print the time that use to execute
  std::cout <<"Time used: " << timeuse << std::endl;
	return 0;
}
