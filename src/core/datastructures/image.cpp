#include <inviwo/core/datastructures/image.h>
#include <inviwo/core/datastructures/imagerepresentation.h>
#include <inviwo/core/datastructures/imagedisk.h>
#include <inviwo/core/datastructures/imageram.h>

namespace inviwo {

Image::Image() : Data2D(uvec2(256,256)) {}

Image::Image(uvec2 dimensions) : Data2D(dimensions) {}

Data* Image::clone() const {
    Image* newImage = new Image(getDimension());
    copyRepresentations(newImage);
    return newImage;
}

Image::~Image() {
    for (size_t i=0; i<representations_.size(); i++) {
        representations_[i]->deinitialize();
    }
    representations_.clear();
}

void Image::resize(uvec2 dimensions) {
    setDimension(dimensions);
    for (size_t i=0; i<representations_.size(); i++) {
        ImageRepresentation* imageRepresentation = dynamic_cast<ImageRepresentation*>(representations_[i]) ;
        if ( imageRepresentation ) {
            imageRepresentation->resize(dimensions);
        }
    }
}

void Image::resizeImageRepresentations(uvec2 targetDim) {
    //TODO: check if getClassName() is necessary.
    //TODO: And also need to be tested on multiple representations_ such as ImageRAM, ImageDisk etc.,
    //TODO: optimize the code
    resize(targetDim);
    ImageRepresentation* imageRepresentation = 0;
    ImageRepresentation* targetRepresentation = 0;
    for (size_t i=0; i<representations_.size(); i++) {
        imageRepresentation = dynamic_cast<ImageRepresentation*>(representations_[i]) ;
        ivwAssert(imageRepresentation!=0, "Only image representations should be used here.");
        for (size_t j=0; j<representations_.size(); j++) {                
            targetRepresentation = dynamic_cast<ImageRepresentation*>(representations_[j]) ;
            ivwAssert(targetRepresentation!=0, "Only image representations should be used here.");
            if (imageRepresentation->getClassName()==targetRepresentation->getClassName()) {
                imageRepresentation->copyAndResizeImage(targetRepresentation);
            }
        }
    }
}

void Image::createDefaultRepresentation() const{
    //representations_.push_back(new ImageDisk());
    representations_.push_back(new ImageRAMVec4uint8());
}

} // namespace
