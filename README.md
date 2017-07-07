# libwann
c++  Weightless Artificial Neural Network library

## Instructions

## How to Install

1. git clone 
2. cd libwann
3. make all (necessary clang++)
4. sudo make install

## How to Use


### WiSARD Signature
```c++

WiSARD(int retinaLength, 
	   int numBitsAddr, 
	   bool useBleaching=true, 
	   float confidenceThreshold=0.1, 
	   int defaultBleaching_b=1, 
	   bool randomizePositions=true, 
	   bool isCummulative=true, 
	   bool ignoreZeroAddr=false);
```


| Parameter           | Description                                                                                   |
| ------------------- | --------------------------------------------------------------------------------------------- |
| retinaLength        | Length of Retina.                                                                             |
| numBitsAddr         | Number of bits to be used for addressing.                                                     |
| useBleaching        | Flag to signal whether or not to use bleaching.                                               |
| confidenceThreshold | Limit value for confidence.                                                                   |
| defaultBleaching_b  | Default value for bleaching.                                                                  |
| randomizePositions  | Flag to signal whether the input mapping should be random.                                    |
| isCummulative       | Flag to signal whether the content of the memories associated with the network is cumulative. |
| ignoreZeroAddr      | Flag to signal if the first address of the memories should be omitted in the analysis.        |

### How to Use


```c++

#include <wann/WiSARD.hpp>
#include <iostream>

using namespace std;
using namespace wann;

int main(){

	vector<vector<int>> training_data = {
		{1,0,1,
		 1,1,1,
		 1,0,1},

		{1,1,1,
		 1,1,0,
		 1,0,0},

		{1,1,1,
		 0,1,0,
		 0,1,0}
	};

	vector<string> training_labels = {"H","F","T"};

	vector<vector<int>> test_data ={
		{1,1,1,
		 1,1,0,
		 0,1,0},

		{1,0,0,
		 1,1,0,
		 1,0,1}
	};

	int retinaLength = training_data[0].size();
	int numBitsAddr = 3;

	WiSARD *w = new WiSARD(retinaLength, numBitsAddr);

	w->fit(training_data, training_labels);

	vector<string> result = w->predict(test_data);

	for (int i = 0; i<test_data.size();i++){
		cout<<"test_data["<<i<<"] seems to be a "<<result[i]<<endl;
	}

	/*The ouput must be:
	  test_data[0] seems to be a T
	  test_data[1] seems to be a H
	*/

	delete w;

	return 0;
}

```
To compile projects with libwann use:

```
clang++ main_source.cpp -o executable_name -std=c++11 -lwann -g -O2 -fpic
```
To generate autodocumentation you will need ```Doxygen``` and ```Graphviz```. With that, just run:
```
doxygen config.doxyfile
```