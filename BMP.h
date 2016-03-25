#ifndef _BMP_H_
#define _BMP_H_
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cstring>
#include <algorithm>
#include <string>
//#include "win-typedefs.h"
#include <random>
#include <thread>
#include <Windows.h>
#include "Utilities.h"
#define BMP CBMP*
using namespace std;

const string Cb_Postfix ( "_Cb" );
const string Cr_Postfix ( "_Cr" );
const string Y_Postfix ( "_Y" );
const string Red_Postfix ( "_red" );
const string Green_Postfix ( "_green" );
const string Blue_Postfix ( "_blue" );
const string Restored_Postfix ( "_restored" );
abstract_class CBMP
{
public:
    CBMP ( );
    CBMP ( const string &filename );
    virtual ~CBMP ( );
    virtual void LoadFile ( );
    virtual void LoadFile ( const string &filename );
    virtual void SaveFile ( );
    virtual void SaveFile ( const string &filename );
    virtual void SetName ( const string &filename );
    virtual void SetFileHeader ( BITMAPFILEHEADER &FileHeader );
    virtual void SetInfoHeader ( BITMAPINFOHEADER &InfoHeader );
    virtual void ShowInfo ( );
    virtual void SetPixelData ( CMatrix2D<rgbD> &obj )
    {
    };
    virtual void SetComponentData ( CMatrix2D<int> &obj, int Component )
    {
    };
    virtual bool isEmpty ( );
    virtual RGBTRIPLE GetPixel ( unsigned y ,unsigned x ) = 0;
    virtual CMatrix2D<int> GetComponentPixmap ( int component ) const = 0;
    virtual CMatrix2D<float> GetComponentData ( int component ) const = 0;
    unsigned GetHeight ( ) const;
    unsigned GetWidth ( ) const;
    virtual void Resize ( unsigned Height ,unsigned Width );
    virtual CMatrix2D<rgbD> GetPixelData ( ) const = 0;
    BITMAPFILEHEADER GetFileHeader ( );
    BITMAPINFOHEADER GetInfoHeader ( );
    string GetFilename ( ) const;
    void AddToFilename ( const string &append );


protected:
    streamoff m_Filesize;
    bool m_bLoadedData;
    void FilenameCheck ( const string &format );
    fstream m_fstFileInput;
    fstream m_fstFileOutput;
    bool m_bHeadersLoad;
    string m_Filename;
    BITMAPFILEHEADER m_sFileHeader;
    BITMAPINFOHEADER m_sInfoHeader;
};
class BMP8 final : public CBMP
{
public:
    BMP8 ( );
    BMP8 ( const BMP8 & );
    BMP8 ( const string & );
    virtual ~BMP8 ( );
    void LoadFile ( );
    void LoadFile ( const string & );
    void SaveFile ( );
    void SaveFile ( const string & );
    void ShowInfo ( );
    RGBTRIPLE GetPixel ( unsigned y , unsigned x );
    CMatrix2D<int> GetComponentPixmap ( int component ) const;
    CMatrix2D<float> GetComponentData ( int component ) const;
    CMatrix2D<rgbD> GetPixelData ( ) const;
private:
    CMatrix2D<BYTE> m_matData;
    vector<RGBQUAD> m_Palette;
};
class BMP24 final : public CBMP
{
public:
    BMP24 ( );
    BMP24 ( const BMP24 &obj );
    BMP24 ( const string &filename );

    virtual ~BMP24 ( );
    void LoadFile ( );
    void LoadFile ( const string &filename );
    void SaveFile ( );
    void SaveFile ( const string &filename );
    void ShowInfo ( );
    void Resize ( unsigned Height , unsigned Width );
    virtual bool isEmpty ( );
    static void SplitIntoColors ( BMP24 &src );
    static void BuildHistogramm ( const string &filename , const CMatrix2D<int> &obj );
    static void BuildHistogramm ( const BMP24 &src );
    static void ConvertToYCbCr ( BMP24 &src , unsigned ratio , bool type );
    CMatrix2D<float> GetComponentData ( int component )const;
    static BMP24 ConvertToRGB ( BMP24 &Y_pic , BMP24 &Cb_pic , BMP24 &Cr_pic );
    virtual void SetPixelData ( CMatrix2D<rgbD> &obj );
    virtual void SetComponentData ( CMatrix2D<int> &obj , int Component );
    RGBTRIPLE GetPixel ( unsigned y , unsigned x );
    CMatrix2D<int> GetComponentPixmap ( int component ) const;
    CMatrix2D<rgbD> GetPixelData ( ) const;
    CMatrix2D<rgbD> *GetInternal ( );
private:
    void recompute_headers ( );
    CMatrix2D<rgbD> m_matData;
};
template<typename _Ty> double ExpectedValue ( const vector<_Ty> &vec )
{
    assert ( vec.size ( ) > 0 );
    double temp = 0;
    for ( auto &i : vec )
    {
        temp += i;
    }
    return temp / ( double ) vec.size ( );
}
double Correlation ( const CMatrix2D<int> &lhs , const CMatrix2D<int> &rhs );
double MeanSquareDisplacement ( const CMatrix2D<int> &obj );
double Correlation ( const CMatrix2D<byte> &lhs , const CMatrix2D<byte> &rhs );
double MeanSquareDisplacement ( const CMatrix2D<byte> &obj );
double PSNR ( CMatrix2D<int> &lhs , CMatrix2D<int> &rhs );
void PSNR_PIC ( CBMP &lhs , CBMP &rhs );
void Autocorrelation ( int y_offset , unsigned step_by_x , const CBMP &image );
void ComputeAutocorrelation ( const BMP24 &image , unsigned step_by_x );
double Entropy ( CMatrix2D<int> &mtrx );
void ShowEntropy ( CBMP &image );
void ShowCorrelation ( CBMP &lhs , CBMP &rhs );
void DPCM_Encoder ( unsigned Mode , BMP24 &image );
CMatrix2D<int> GenerateGaussianNoise ( float sigma , unsigned Height , unsigned Width );
// double random ( );
double N ( double M , double G );
void ShowHistogramm ( string filename , double s_range , double e_range , vector<double> &vec );
int BurstNoise ( double Pa , double Pb );
double Dispersion ( const CMatrix2D<byte> &obj );
CMatrix2D<rgbD> fromVector ( vector<unsigned char> &vec , size_t height , size_t width );
vector<unsigned char> fromImage ( BMP24 &img );
vector<unsigned char> fromMatrix ( CMatrix2D<rgbD> &mtrx );
std::vector<unsigned char> bicubicresize ( const std::vector<unsigned char>& in , std::size_t src_width , std::size_t src_height , std::size_t dest_width , std::size_t dest_height );
inline unsigned char getpixel ( const std::vector<unsigned char>& in , std::size_t src_width , std::size_t src_height , unsigned y , unsigned x , int channel );
//CMatrix2D<rgbD> zoom ( CMatrix2D<rgbD>& image , int f );
CMatrix2D<rgbD> InterpolateBicubic ( CMatrix2D<rgbD>& input , double x_scale , double y_scale );
double bicubicInterpolate ( double p [ 4 ] [ 4 ] , double x , double y );
double cubicInterpolate ( double p [ 4 ] , double x );
void downscalebytwo ( CMatrix2D<rgbD>& image );

static CBMP *LoadBMP ( string &filename ) // FactoryMethod
{
    try
    {
        if ( !strstr ( filename.c_str ( ) , ".bmp" ) )
        {
#ifdef _DEBUG
            cout << "Filename fixed\n";
#endif

            filename += ".bmp";
        }
        fstream input ( filename , ios::in | ios::binary );
        if ( input.is_open ( ) )
        {
            input.seekg ( 0 , input.end );
            streamoff filesize = input.tellg ( );
            if ( filesize < ( sizeof ( BITMAPFILEHEADER ) + sizeof ( BITMAPINFOHEADER ) ) )
            {
                input.close ( );
                throw runtime_error ( "Error occured! File size can't be valid bmp file" );
            }
            input.seekg ( input.beg );
            BITMAPFILEHEADER fileheader;
            BITMAPINFOHEADER infoheader;
            input.read ( reinterpret_cast< char* >( &fileheader ) , sizeof ( BITMAPFILEHEADER ) );
            if ( fileheader.bfType != 'MB' )
            {
                input.close ( );
                throw runtime_error ( "Error occured! This is not valid bmp file!" );
            }
            input.read ( reinterpret_cast< char* >( &infoheader ) , sizeof ( BITMAPINFOHEADER ) );
            switch ( infoheader.biBitCount )
            {
                case bit_depth_8:
                    input.close ( );
                    return new BMP8 ( filename );
                case bit_depth_24:
                    input.close ( );
                    return new BMP24 ( filename );
                default:
                    throw runtime_error ( "Error! Invalid bit depth" );
                    input.close ( );
            }
        }
        else
        {
            throw runtime_error ( "Error! Couldn't open file : " + filename );
        }
    }
    catch ( runtime_error &err )
    {
        cout << err.what ( ) << endl;
    }
    return nullptr;
}
#endif
