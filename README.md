### Install

`brew install opencv`

### HEADER

```
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
```

### RUN
`g++ $(pkg-config --cflags --libs /usr/local/Cellar/opencv/4.1.2/lib/pkgconfig/opencv4.pc) -std=c++11 hello.cpp -o hello`

