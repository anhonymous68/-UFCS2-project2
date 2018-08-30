#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

float Clamp(float a)
{
    if (a > 255)
        return 255;
    else if ( a < 0)
        return 0;
    else
        return a;
}

struct Pixel
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct Header
{
    char idLength;
    char colourMapType;
    char dataTypeCode;
    short colourMapOrigin;
    short colourMapLength;
    char colourMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
    void Display()
    {
        cout<<"Id Length: "<<idLength<<endl;
        cout<<"Colour map type: "<<colourMapType<<endl;
        cout<<"Data type code: "<<dataTypeCode<<endl;
        cout<<"Colour Map Origin: "<<colourMapOrigin<<endl;
        cout<<"Colour Map Length: "<<colourMapLength<<endl;
        cout<<"Colour Map Depth: "<<colourMapDepth<<endl;
        cout<<"X Origin: "<<xOrigin<<endl;
        cout<<"Y Origin: "<<yOrigin<<endl;
        cout<<"Width: "<<width<<endl;
        cout<<"Height: "<<height<<endl;
        cout<<"Bits per pixel: "<<bitsPerPixel<<endl;
        cout<<"Image Descriptor: "<<imageDescriptor<<endl;
        cout<<endl;
    }
};

void WriteBinaryData(string strFile, Header * p, vector<Pixel> &pixelArray, vector<string>  * outF)
{
    outF->push_back(strFile);
    ofstream binaryFile;
    binaryFile.open(strFile,  ios::binary);
    if (!binaryFile.is_open())
    {
        cout << "cannot open file" << strFile << endl;
    }
    else
    {
        binaryFile.write(&p->idLength, 1);
        binaryFile.write( &p->colourMapType, 1);
        binaryFile.write( &p->dataTypeCode, 1);
        binaryFile.write((char *) &p->colourMapOrigin, 2);
        binaryFile.write((char *) &p->colourMapLength, 2);
        binaryFile.write( &p->colourMapDepth, 1);
        binaryFile.write((char *) &p->xOrigin, 2);
        binaryFile.write((char *) &p->yOrigin, 2);
        binaryFile.write((char *) &p->width, 2);
        binaryFile.write((char *) &p->height, 2);
        binaryFile.write( &p->bitsPerPixel, 1);
        binaryFile.write( &p->imageDescriptor, 1);
        
        for (unsigned int i = 0; i < pixelArray.size(); i++)
        {
            binaryFile.write((char*) +&pixelArray[i].blue, 1);
            binaryFile.write((char*) +&pixelArray[i].green, 1);
            binaryFile.write((char*) +&pixelArray[i].red, 1);
        }
    }
    binaryFile.close();
}

vector<Pixel> MultiplyBlend(vector<Pixel> &A, vector<Pixel> &B)
{
    vector<Pixel> result;
    for (unsigned int i = 0; i < A.size(); i++)
    {
        Pixel temp;
        temp.red = Clamp(((((float)(+A[i].red)/255.0f) * ((B[i].red)/255.0f)) * 255.0f) + 0.5f);
        temp.green = Clamp(((((float)(+A[i].green)/255.0f) * (((float)+B[i].green)/255.0f)) * 255.0f) + 0.5f);
        temp.blue = Clamp(((((float)(+A[i].blue)/255.0f) * (((float)+B[i].blue)/255.0f)) * 255.0f) + 0.5f);
        
        result.push_back(temp);
    }
    return result;
}

vector<Pixel> SubtractBlend(vector<Pixel> &A, vector<Pixel> &B)
{
    vector<Pixel> result;
    for (unsigned int i = 0; i < A.size(); i++)
    {
        Pixel temp;
        temp.red = Clamp(+A[i].red - +B[i].red);
        temp.green = Clamp(+A[i].green - +B[i].green);
        temp.blue = Clamp(+A[i].blue - +B[i].blue);
        result.push_back(temp);
    }
    return result;
}

vector<Pixel> ScreenBlend(vector<Pixel> &A, vector<Pixel> &B)
{
    vector<Pixel> result;
    for (unsigned int i = 0; i < A.size(); i++)
    {
        Pixel temp;
        float ar = (float)(+A[i].red)/255.0f;
        float br = (float)(+B[i].red)/255.0f;
        temp.red = (1.0f - (1 - ar) * (1 - br)) * 255.0f + 0.5f;
        float ag = (float)(+A[i].green)/255.0f;
        float bg = (float)(+B[i].green)/255.0f;
        temp.green = (1.0f - (1 - ag) * (1 - bg)) * 255.0f + 0.5f;
        float ab = (float)(+A[i].blue)/255.0f;
        float bb = (float)(+B[i].blue)/255.0f;
        temp.blue = (1.0f - (1 - ab) * (1 - bb)) * 255.0f + 0.5f;
        result.push_back(temp);
    }
    return result;
}

int OverlayBlendFormula(int A, int B)
{
    float tempA = (float)A / 255.0f;
    float tempB = (float)B / 255.0f;
    float c;
    if (tempB <= 0.5)
    {
        c = 2 * tempA * tempB;
        c = c * 255.0f + 0.5f;
    }
    else
    {
        c = 1 - 2 *( 1 - tempA) * (1 - tempB);
        c = c * 255.0f + 0.5f;
    }
    return (int)c;
}
vector<Pixel> OverlayBlend(vector<Pixel> &A, vector<Pixel> &B)
{
    vector<Pixel> result;
    for (unsigned int i = 0; i < A.size(); i++)
    {
        Pixel temp;
        temp.red = OverlayBlendFormula(A[i].red, B[i].red);
        temp.green = OverlayBlendFormula(A[i].green, B[i].green);
        temp.blue = OverlayBlendFormula(A[i].blue, B[i].blue);
        
        result.push_back(temp);
    }
    return result;
}

vector<Pixel> UpSideDown(vector<Pixel> &A)
{
    vector<Pixel> result;
    for (unsigned int i = 0; i < A.size(); i++)
    {
        result.push_back( A[A.size() - i - 1]);
    }
    return result;
}

void ReadBinaryFile(string nameFile, Header  * p, vector<Pixel> &pixelArray)
{
    ifstream binaryFile(nameFile, ios_base::binary | ios_base::in);
    binaryFile.read(&p->idLength, 1);
    binaryFile.read((char *) &p->colourMapType, 1);
    binaryFile.read((char *) &p->dataTypeCode, 1);
    binaryFile.read((char *) &p->colourMapOrigin, 2);
    binaryFile.read((char *) &p->colourMapLength, 2);
    binaryFile.read((char *) &p->colourMapDepth, 1);
    binaryFile.read((char *) &p->xOrigin, 2);
    binaryFile.read((char *) &p->yOrigin, 2);
    binaryFile.read((char *) &p->width, 2);
    binaryFile.read((char *) &p->height, 2);
    binaryFile.read((char *) &p->bitsPerPixel, 1);
    binaryFile.read((char *) &p->imageDescriptor, 1);
    
    int arraySize = p->width * p->height;
    for (unsigned int i = 0; i < arraySize; i++)
    {
        Pixel temp;
        binaryFile.read((char*)&temp.blue, 1);
        binaryFile.read((char*)&temp.green, 1);
        binaryFile.read((char*)&temp.red, 1);
        pixelArray.push_back(temp);
    }
}


void Test(vector<string> outputFiles, vector<string> expectedFiles)
{
    int totalPoints = 0;
    for (unsigned i = 0; i < expectedFiles.size(); i++)
    {
        vector<Pixel> outP;
        Header outH;
        ReadBinaryFile(outputFiles[i],&outH,outP);
        vector<Pixel> exP;
        Header exH;
        ReadBinaryFile(expectedFiles[i], &exH, exP);
        if (exP.size() != outP.size())
        {
            cout<<"Part "<< i+ 1<<" not successful! + 0 points"<<endl;
            break;
        }
        else
        {
            bool correct = true;
            for (unsigned int j = 0; j < outP.size(); j++)
            {
                if (exP[i].red != outP[i].red || exP[i].green != outP[i].green || exP[i].blue != outP[i].blue)
                {
                    correct = false;
                    break;
                }
            }
            if (correct == false)
            {
                cout<<"Part "<<i + 1<<" not successful! + 0 points"<<endl;
            }
            else
            {
                totalPoints += 20;
                cout<<"Part " <<i + 1<<" successful! + 20 points"<<endl;
            }
        }
        
    }
    cout<<"Total score: "<< totalPoints<<endl;
}


int main()
{
    vector<string> outF;
    vector<string> expF;
    for (int i = 1; i < 8; i++)
    {
        int a = 0;
        string temp = "/Users/hoangducanh11031995/project2/ImageProcessing/examples/EXAMPLE_part" +  to_string(i);
        temp = temp + ".tga";
        expF.push_back(temp);
    }
    
    expF.push_back("/Users/hoangducanh11031995/project2/ImageProcessing/examples/EXAMPLE_part8_r.tga");
    expF.push_back("/Users/hoangducanh11031995/project2/ImageProcessing/examples/EXAMPLE_part8_g.tga");
    expF.push_back("/Users/hoangducanh11031995/project2/ImageProcessing/examples/EXAMPLE_part8_b.tga");
    
    for (int i = 9; i < 11; i++)
    {
        int a = 0;
        string temp = "/Users/hoangducanh11031995/project2/ImageProcessing/examples/EXAMPLE_part" +  to_string(i);
        temp = temp + ".tga";
        expF.push_back(temp);
    }
    
    
    //Part 1:
    //Use the Multiply blending mode to combine “layer1.tga” (top layer) with “pattern1.tga” (bottom layer);
    
    Header layer1;
    vector<Pixel> pLayer1;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/layer1.tga", &layer1, pLayer1);
    
    Header pattern1;
    vector<Pixel> ppattern1;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/pattern1.tga", &pattern1, ppattern1);
    vector<Pixel> pPart1 = MultiplyBlend(ppattern1, pLayer1);
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part1.tga", &pattern1, pPart1, &outF);
    
    
    //P A R T  2
    Header car;
    vector<Pixel> pCar;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/car.tga", &car, pCar);
    Header layer2;
    vector<Pixel> pLayer2;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/layer2.tga", &layer2, pLayer2);
    
    
    vector<Pixel> arrayForPart2 = SubtractBlend(pCar, pLayer2);
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part2.tga", &car, arrayForPart2, &outF);
    
    
    //part 3
    
    vector<Pixel> pPattern2;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/pattern2.tga", &layer2, pPattern2);
    
    vector<Pixel> part3_a = MultiplyBlend(pLayer1, pPattern2);
    vector<Pixel> pText;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/text.tga", &layer2, pText);
    
    vector<Pixel> pPart3 = ScreenBlend(pText,part3_a);
    
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part3.tga", &car, pPart3,&outF);
    
    
    //part 4 Multiply “layer2.tga” with “circles.tga”, and store it. Load “pattern2.tga” and, using that as the top
    // layer, combine it with the previous result using the Subtract blending mode.
    vector<Pixel> pCircle;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/circles.tga", &layer2, pCircle);
    vector<Pixel> p4a = MultiplyBlend(pCircle, pLayer2);
    vector<Pixel> pPart4 = SubtractBlend(  p4a, pPattern2);
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part4.tga", &car, pPart4, &outF);
    
    //part 5
    vector<Pixel> pPart5 = OverlayBlend(pLayer1, ppattern1);
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part5.tga", &car, pPart5, &outF);
    
    //part 6
    //Load “car.tga” and add 200 to the green channel.
    vector<Pixel> pPart6;
    for (unsigned int i = 0; i < pCar.size(); i++)
    {
        Pixel temp;
        temp.red = +pCar[i].red;
        temp.green = Clamp(+pCar[i].green + 200);
        temp.blue = +pCar[i].blue;
        pPart6.push_back(temp);
    }
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part6.tga", &car, pPart6, &outF);
    
    //part 7
    //Load “car.tga” and scale the red channel by 4, and the blue channel by 0.
    vector<Pixel> pPart7;
    Header hPart7;
    vector<Pixel> pCarP7;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/car.tga", &hPart7, pCarP7);
    
    for (unsigned int i = 0; i < pCar.size(); i++)
    {
        Pixel temp;

        temp.red = Clamp((pCarP7[i].red / 255.0f) * 4.0f * 255 + 0.5f);
        temp.green = pCarP7[i].green;
        temp.blue = Clamp((pCarP7[i].blue / 255.0f) * 0.0f * 255 + 0.5f);;
        pPart7.push_back(temp);
    }
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part7.tga", &hPart7, pPart7, &outF);
    
    
    //8. Load “car.tga” and write each channel to a separate file: the red channel should be “part8_r.tga”,
    // the green channel should be “part8_g.tga”, and the blue channel should be “part8_b.tga”
    
    vector<Pixel> pPart8r;
    vector<Pixel> pPart8b;
    vector<Pixel> pPart8g;
    
    for (unsigned int i = 0; i < pCar.size(); i++)
    {
        Pixel temp;
        temp.red = +pCar[i].red;
        temp.blue = +pCar[i].red;
        temp.green = +pCar[i].red;
        pPart8r.push_back(temp);
    }
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part8_r.tga", &car, pPart8r, &outF);
    
    for (unsigned int i = 0; i < pCar.size(); i++)
    {
        Pixel temp;
        temp.green = +pCar[i].green;
        temp.blue = +pCar[i].green;
        temp.red = +pCar[i].green;
        pPart8g.push_back(temp);
    }
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part8_g.tga", &car, pPart8g, &outF);
    
    for (unsigned int i = 0; i < pCar.size(); i++)
    {
        Pixel temp;
        temp.blue = +pCar[i].blue;
        temp.red = +pCar[i].blue;
        temp.green = +pCar[i].blue;
        pPart8b.push_back(temp);
    }
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part8_b.tga", &car, pPart8b, &outF);
    
    //9. Load “layer_red.tga”, “layer_green.tga” and “layer_blue.tga”, and combine the three files into one file.
    // The data from “layer_red.tga” is the red channel of the new image, layer_green is green, and layer_blue is blue.
    
    Header hPart9;
    vector<Pixel> pPart9;
    vector<Pixel> pLayerRed;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/layer_red.tga", &hPart9, pLayerRed);
    vector<Pixel> pLayerGreen;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/layer_green.tga", &hPart9, pLayerGreen);
    vector<Pixel> pLayerBlue;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/layer_blue.tga", &hPart9, pLayerBlue);
    
    for (unsigned int i = 0; i < pLayerGreen.size(); i++)
    {
        Pixel temp;
        temp.red = pLayerRed[i].red;
        temp.green = pLayerGreen[i].green;
        temp.blue = pLayerBlue[i].blue;
        
        pPart9.push_back(temp);
    }
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part9.tga", &hPart9, pPart9, &outF);
    
    //    10. Load “text2.tga”, and rotate it 180 degrees, flipping it upside down. This is easier than you think! Try
    // diagramming the data of an image (such as earlier in this document). What would the data look like if you flipped it?
    // Now, how to write some code to accomplish that…?
    
    Header example2;
    vector<Pixel> pExample2;

    vector<Pixel> text2UpDown;
    ReadBinaryFile("/Users/hoangducanh11031995/project2/ImageProcessing/input/text2.tga", & example2, text2UpDown);
    text2UpDown = UpSideDown(text2UpDown);
    WriteBinaryData("/Users/hoangducanh11031995/project2/ImageProcessing/output/part10.tga", &example2, text2UpDown, &outF);
    
    Test(outF, expF);


}
