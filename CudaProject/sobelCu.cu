#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>

// store data of image
unsigned char *data_rgb_image;

// cuda kernel
__global__ void RGBtoGrey(unsigned char *rgb, unsigned char *grey, int _row, int _col) {

    int col = threadIdx.x + blockIdx.x * blockDim.x;
    int row = threadIdx.y + blockIdx.y * blockDim.y;

    // calculate for only in the GridDim
    if (col < _col && row < _row) {
        // calculate Global threadID
      const int grey_offset = row * _col + col;
        // 3 mean channel (R G B)
      const int rgb_offset = grey_offset * 3;
        // use unsigned char because size of it is 0 - 255 that mach with color (0-255)
    	const unsigned char r = rgb[rgb_offset + 0]; // 0 for red
	    const unsigned char g = rgb[rgb_offset + 1]; // 1 for green
	    const unsigned char b = rgb[rgb_offset + 2]; // 2 for blue

	    grey[grey_offset] = r * 0.21f + g * 0.71f + b * 0.07f;
    }
}

__global__ void GreytoSobel(unsigned char *gray, unsigned char *sobelX, unsigned char *sobelY, int _row, int _col) {
    int col = threadIdx.x + blockIdx.x * blockDim.x;
    int row = threadIdx.y + blockIdx.y * blockDim.y;
    if ( col< _col && row < _row) {
        // calculate Global threadID
        int sobel_offset = row * _col + col;
        int margin[3][3];
        // Gx for mask of horizontal
        int Gx[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};
        // Gy for mask of vertical
        int Gy[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
        //calculate margin for 3 x 3 matrix (neighbor of pixel)
        margin[0][0] = (row-1) * _col + col - 1;
        margin[0][1] = (row-1) * _col + col;
        margin[0][2] = (row-1) * _col + col + 1;
        margin[1][0] = row * _col + col - 1;
        margin[1][1] = row * _col + col;
        margin[1][2] = row * _col + col + 1;
        margin[2][0] = (row+1) * _col + col - 1;
        margin[2][1] = (row+1) * _col + col;
        margin[2][2] = (row+1) * _col + col + 1;

        int sumX = 0, sumY = 0;
        // calcurate sobel
        for(int i = 0; i < 3; i ++){
            for(int j = 0; j < 3; j++){
                sumX += gray[margin[i][j]] * Gx[i][j];
                sumY += gray[margin[i][j]] * Gy[i][j];
            }
        }
        if(sumX >= 255){
            sumX = 255;
        }
        if(sumY >= 255){
            sumY = 255;
        }
        if(sumX <= 0){
            sumX = 0;
        }
        if(sumY <= 0){
            sumY = 0;
        }
        // printf("%d\n", sumX);
        sobelX[sobel_offset] = sumX;
        sobelY[sobel_offset] = sumY;
    }
}
//function for loading an image into rgb format unsigned char array
size_t loadImgFile(unsigned char *grey_image, const std::string &input_file, int *rows, int *cols) {

    cv::Mat img_data; //opencv Mat object
    //read image data into img_data Mat object
    img_data = cv::imread(input_file.c_str(), cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
    if (img_data.empty()) {
        std::cerr << "Unable to laod image file: " << input_file << std::endl;
    }

    *rows = img_data.rows;
    *cols = img_data.cols;

    //allocate memory for host rgb data array
    data_rgb_image = (unsigned char*) malloc(*rows * *cols * sizeof(unsigned char) * 3);
    unsigned char* rgb_image = (unsigned char*)img_data.data;

    //populate host's rgb data array
    int x = 0;
    for (x = 0; x < *rows * *cols * 3; x++) {
        data_rgb_image[x] = rgb_image[x];
    }
    // return size of image in pixel
    return img_data.rows * img_data.cols;
}

// method for writing image file of gray scale
void outputImg(const std::string& output_file, unsigned char* grey_image, int _row, int _col) {

	//serialize gray data array into opencv's Mat object
	cv::Mat greyData(_row, _col, CV_8UC1,(void *) grey_image);
	//write Mat object to file
    cv::imwrite(output_file.c_str(), greyData);
    printf("Transfer complete\n");
}

// size_t (unsigned integer)
size_t loadImgFile(unsigned char *grey_img, const std::string &input_file, int *_row, int *_col );
void outputImg(const std::string &output_file, unsigned char *grey_image, int _row, int _col);

int main(int argc, char **argv) {

    clock_t begin = clock();
	  std::string input_file;
    std::string gray_file;
    std::string horizontal_sobel_file;
    std::string vertical_sobel_file;

	//Check for the input file and output file names
	switch(argc) {
		case 5:
			      input_file = std::string(argv[1]);
            gray_file = std::string(argv[2]);
            horizontal_sobel_file = std::string(argv[3]);
            vertical_sobel_file = std::string(argv[4]);
            break;
		default:
			std::cerr << "Usage: <executable> input_file gray_file horizontal_sobel_file vertical_sobel_file\n";
			exit(1);
	}

	unsigned char *d_rgb_image; //array for storing rgb data on device
    unsigned char *data_grey_image = 0;
    unsigned char *data_sobel_x_image = 0;
    unsigned char *data_sobel_y_image = 0;
    unsigned char *d_grey_image = 0; //host and device's grey data array pointers
    unsigned char *d_sobel_x = 0;
    unsigned char *d_sobel_y = 0;
	int p_rows; //number of rows of pixels
	int p_cols; //number of columns of pixels

	//load image into an array and retrieve number of pixels
    const size_t total_pixels = loadImgFile(data_grey_image, input_file, &p_rows, &p_cols);

	//allocate memory of host's grey data array
    data_grey_image = (unsigned char *)malloc(sizeof(unsigned char*)* total_pixels);
    data_sobel_x_image = (unsigned char *)malloc(sizeof(unsigned char*)* total_pixels);
    data_sobel_y_image = (unsigned char *)malloc(sizeof(unsigned char*)* total_pixels);

	//allocate and initialize memory on device
	cudaMalloc(&d_rgb_image, sizeof(unsigned char) * total_pixels * 3); // 3 is for channel (R G B)
    cudaMalloc(&d_grey_image, sizeof(unsigned char) * total_pixels);
    cudaMalloc(&d_sobel_x, sizeof(unsigned char) * total_pixels);
    cudaMalloc(&d_sobel_y, sizeof(unsigned char) * total_pixels);
	cudaMemset(d_grey_image, 0, sizeof(unsigned char) * total_pixels);

	//copy host rgb data array to device data array
    cudaMemcpy(d_rgb_image, data_rgb_image, sizeof(unsigned char) * total_pixels * 3, cudaMemcpyHostToDevice);
    cudaMemcpy(d_sobel_x, data_sobel_x_image, sizeof(unsigned char) * total_pixels, cudaMemcpyHostToDevice);
    cudaMemcpy(d_sobel_y, data_sobel_y_image, sizeof(unsigned char) * total_pixels, cudaMemcpyHostToDevice);

	//define block and grid dimensions
	const dim3 dimGrid((int)ceil((p_cols)/16), (int)ceil((p_rows)/16));
	const dim3 dimBlock(16, 16);

	//execute cuda kernel
    RGBtoGrey<<<dimGrid, dimBlock>>>(d_rgb_image, d_grey_image, p_rows, p_cols);
    GreytoSobel<<<dimGrid, dimBlock>>>(d_grey_image,d_sobel_x, d_sobel_y, p_rows, p_cols);

	//copy computed gray data array from device to host
    cudaMemcpy(data_grey_image, d_grey_image, sizeof(unsigned char) * total_pixels, cudaMemcpyDeviceToHost);
    cudaMemcpy(data_sobel_x_image, d_sobel_x, sizeof(unsigned char) * total_pixels, cudaMemcpyDeviceToHost);
    cudaMemcpy(data_sobel_y_image, d_sobel_y, sizeof(unsigned char) * total_pixels, cudaMemcpyDeviceToHost);

	//output the grayscale image
    outputImg(gray_file, data_grey_image, p_rows, p_cols);
    outputImg(horizontal_sobel_file, data_sobel_x_image, p_rows, p_cols);
    outputImg(vertical_sobel_file, data_sobel_y_image, p_rows, p_cols);

    cudaFree(d_rgb_image);
    cudaFree(d_grey_image);
    cudaFree(d_sobel_x);
    cudaFree(d_sobel_y);

    clock_t end = clock();
    double elapsed_secs = double(end - begin);
    fflush(stdout);
    // printf("Time used: %ls",elapsed_secs);
    // printf("Error %d: %s.\n", id, errors[id]);
    // std::cout << "Error " << id << ": " << errors[id] << "." << std::endl;
    std::cout <<"Time used: " << elapsed_secs << std::endl;
	return 0;
}
