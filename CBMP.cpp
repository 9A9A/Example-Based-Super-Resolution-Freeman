#include "BMP.h"
CBMP::CBMP ( )
{
    m_bHeadersLoad = false;
    m_bLoadedData = false;
}
CBMP::CBMP ( const string &filename ) : CBMP ( )
{
    CBMP::LoadFile ( filename );
}
CBMP::~CBMP ( )
{
    m_fstFileInput.close ( );
    m_fstFileOutput.close ( );
#ifdef _DEBUG
    cout << m_Filename << " - Abstract Base Object has been destroyed.\n";
#endif
    m_Filename.clear ( );
}
unsigned CBMP::GetHeight ( ) const
{
    return m_sInfoHeader.biHeight;
}
unsigned CBMP::GetWidth ( ) const
{
    return m_sInfoHeader.biWidth;
}
void CBMP::FilenameCheck ( const string &format )
{
    if ( !strstr ( m_Filename.c_str ( ) , format.c_str ( ) ) )
    {
#ifdef _DEBUG
        cout << "Filename changed : " << m_Filename;
#endif
        m_Filename += format;
#ifdef _DEBUG
        cout << " to " << m_Filename << endl;
#endif
    }
}
string CBMP::GetFilename ( ) const
{
    string filename = m_Filename;
    if ( strstr ( m_Filename.c_str ( ) , ".bmp" ) )
    {
        filename.erase ( filename.end ( ) - 4 , filename.end ( ) );
    }
    return filename;
}
void CBMP::ShowInfo ( )
{
    if ( m_bHeadersLoad )
    {
        cout << "Resolution : " << m_sInfoHeader.biWidth << "x" << m_sInfoHeader.biHeight << " \\ bit depth: " << m_sInfoHeader.biBitCount << endl;
        cout << "BITMAPFILEHEADER\n";
        cout << "\tOffBits: " << m_sFileHeader.bfOffBits << endl;
        cout << "\tReserved1: " << m_sFileHeader.bfReserved1 << endl;
        cout << "\tReserved2: " << m_sFileHeader.bfReserved2 << endl;
        cout << "\tSize: " << m_sFileHeader.bfSize << endl;
        cout << "\tType: " << m_sFileHeader.bfType << endl;
        cout << "BITMAPINFOHEADER\n";
        cout << "\tBitCount: " << m_sInfoHeader.biBitCount << endl;
        cout << "\tClrImportant: " << m_sInfoHeader.biClrImportant << endl;
        cout << "\tClrUsed: " << m_sInfoHeader.biClrUsed << endl;
        cout << "\tCompression: " << m_sInfoHeader.biCompression << endl;
        cout << "\tHeight: " << m_sInfoHeader.biHeight << endl;
        cout << "\tWidth: " << m_sInfoHeader.biWidth << endl;
        cout << "\tPlanes: " << m_sInfoHeader.biPlanes << endl;
        cout << "\tSize: " << m_sInfoHeader.biSize << endl;
        cout << "\tSizeImage: " << m_sInfoHeader.biSizeImage << endl;
        cout << "\tXPelsPerMeter: " << m_sInfoHeader.biXPelsPerMeter << endl;
        cout << "\tYPelsPerMeter: " << m_sInfoHeader.biYPelsPerMeter << endl;
    }
    else
    {
        cout << "File is empty\n";
    }
}
void CBMP::LoadFile ( const string &filename )
{
    m_Filename = filename;
    FilenameCheck ( string ( ".bmp" ) );
    CBMP::LoadFile ( );
}
void CBMP::AddToFilename ( const string &append )
{
    if ( strstr ( m_Filename.c_str ( ) , ".bmp" ) )
    {
#ifdef _DEBUG
        cout << "Adding\n";
#endif
        m_Filename.erase ( m_Filename.end ( ) - 4 , m_Filename.end ( ) );
    }
    m_Filename += append;
    FilenameCheck ( string ( ".bmp" ) );
}
bool CBMP::isEmpty ( )
{
    return m_bLoadedData;
}
void CBMP::LoadFile ( )
{
    try
    {
        m_fstFileInput.open ( m_Filename , ios::in | ios::binary );
        if ( m_fstFileInput.is_open ( ) )
        {
#ifdef _DEBUG
            cout << "File : " << m_Filename << " is opened, reading...\n";
#endif
            m_fstFileInput.seekg ( 0 , m_fstFileInput.end );
            m_Filesize = m_fstFileInput.tellg ( );
#ifdef _DEBUG
            cout << "filesize : " << m_Filesize << endl;
#endif
            m_fstFileInput.seekg ( m_fstFileInput.beg );
            m_fstFileInput.read ( reinterpret_cast< char* >( &m_sFileHeader ) , sizeof ( BITMAPFILEHEADER ) );
            m_fstFileInput.read ( reinterpret_cast< char* >( &m_sInfoHeader ) , sizeof ( BITMAPINFOHEADER ) );
            m_bHeadersLoad = true;
        }
        else
        {
            throw runtime_error ( string ( "Error! Couldn't open file : " ) + m_Filename );
        }
    }
    catch ( runtime_error &err )
    {
        cout << err.what ( ) << endl;
    }
}
void CBMP::SetName ( const string &filename )
{
    m_Filename = filename;
    FilenameCheck ( string ( ".bmp" ) );
}
void CBMP::SetFileHeader ( BITMAPFILEHEADER &FileHeader )
{
    m_sFileHeader = FileHeader;
}
void CBMP::SetInfoHeader ( BITMAPINFOHEADER &InfoHeader )
{
    m_sInfoHeader = InfoHeader;
}
void CBMP::SaveFile ( const string &filename )
{
    m_Filename = filename;
    FilenameCheck ( string ( ".bmp" ) );
    CBMP::SaveFile ( );
}
void CBMP::SaveFile ( )
{
    try
    {
        m_fstFileOutput.open ( m_Filename , ios::out | ios::binary );
        if ( m_fstFileOutput.is_open ( ) && m_bHeadersLoad )
        {
#ifdef _DEBUG
            cout << m_Filename << " - Writing headers...\n";
#endif
            m_fstFileOutput.write ( reinterpret_cast< char* >( &m_sFileHeader ) , sizeof ( BITMAPFILEHEADER ) );
            m_fstFileOutput.write ( reinterpret_cast< char* >( &m_sInfoHeader ) , sizeof ( BITMAPINFOHEADER ) );
        }
        else
        {
            throw runtime_error ( "Error! Couldn't open file : " + m_Filename );
        }
    }
    catch ( runtime_error &err )
    {
        cout << err.what ( );
    }
}
BITMAPFILEHEADER CBMP::GetFileHeader ( )
{
    return m_sFileHeader;
}
BITMAPINFOHEADER CBMP::GetInfoHeader ( )
{
    return m_sInfoHeader;
}
void CBMP::Resize ( unsigned height , unsigned Width )
{
}
inline unsigned char getpixel ( const std::vector<unsigned char>& in ,
                                std::size_t src_width , std::size_t src_height , unsigned y , unsigned x , int channel )
{
    if ( x < src_width && y < src_height )
        return in [ ( y * 3 * src_width ) + ( 3 * x ) + channel ];
    return 0;
}

double cubicInterpolate ( double p [ 4 ] , double x )
{
    return p [ 1 ] + 0.5 * x*( p [ 2 ] - p [ 0 ] + x*( 2.0*p [ 0 ] - 5.0*p [ 1 ] + 4.0*p [ 2 ] - p [ 3 ] + x*( 3.0*( p [ 1 ] - p [ 2 ] ) + p [ 3 ] - p [ 0 ] ) ) );
}

double bicubicInterpolate ( double p [ 4 ] [ 4 ] , double x , double y )
{
    double arr [ 4 ];
    arr [ 0 ] = cubicInterpolate ( p [ 0 ] , y );
    arr [ 1 ] = cubicInterpolate ( p [ 1 ] , y );
    arr [ 2 ] = cubicInterpolate ( p [ 2 ] , y );
    arr [ 3 ] = cubicInterpolate ( p [ 3 ] , y );
    return cubicInterpolate ( arr , x );
}
CMatrix2D<rgbD> InterpolateBicubic ( CMatrix2D<rgbD>& input , double x_scale , double y_scale )
{
    vector<byte> x = fromMatrix ( input );
    cv::Mat inputMatrix ( input.GetHeight ( ) , input.GetWidth ( ) , CV_8UC3 , x.data ( ) );
    cv::InputArray temp_pic ( inputMatrix );
    cv::OutputArray output_pic ( inputMatrix );
    cv::resize ( temp_pic , output_pic , cv::Size(0,0) , x_scale , y_scale , CV_INTER_CUBIC );
    auto matToBytes = [ ] ( cv::Mat& image )->vector<byte>
    {
        int size = image.total ( )*image.elemSize ( );
        byte *bytes = new byte [ size ];
        memcpy ( bytes , image.data , size*sizeof ( byte ) );
        vector<byte> output;
        for ( auto i = 0; i < size; i++ )
        {
            output.push_back ( bytes [ i ] );
        }
        delete [ ] bytes;
        return output;
    };
    return fromVector ( matToBytes ( inputMatrix ) , inputMatrix.size ( ).height , inputMatrix.size ( ).width );
}
void downscalebytwo ( CMatrix2D<rgbD>& image )
{
    CMatrix2D<rgbD> temp ( image.GetHeight ( ) / 2 , image.GetWidth ( ) / 2 );
    size_t counter = 0;
    for ( size_t i = 0; i < image.GetHeight ( ); i += 2 )
    {
        for ( size_t j = 0; j < image.GetWidth ( ); j += 2 )
        {
            CMatrix2D<rgbD> rect = image.rect ( i , j , 2 , 2 );
            double red = 0 , green = 0 , blue = 0;
            for ( auto &i : rect )
            {
                red += i.Red;
                green += i.Green;
                blue += i.Blue;
            }
            red /= 4.0;
            blue /= 4.0;
            green /= 4.0;
            check_bounds ( red );
            check_bounds ( blue );
            check_bounds ( green );
            temp [ counter++ ] = rgbD ( red , green , blue );
        }
    }
    image = move ( temp );
}
std::vector<unsigned char> bicubicresize ( const std::vector<unsigned char>& in ,
                                           std::size_t src_width , std::size_t src_height , std::size_t dest_width , std::size_t dest_height )
{
 /*   std::vector<unsigned char> out ( dest_width * dest_height * 3 );

    const float tx = float ( src_width ) / dest_width;
    const float ty = float ( src_height ) / dest_height;
    const int channels = 3;
    const std::size_t row_stride = dest_width * channels;

    double C [ 5 ] = { 0 };

    for ( int i = 0; i < dest_height; ++i )
    {
        for ( int j = 0; j < dest_width; ++j )
        {
            const float x = float ( tx * j );
            const float y = float ( ty * i );
            const float dx = tx * j - x;
            const float dy = ty * i - y;

            for ( int k = 0; k < 3; ++k )
            {
                for ( int jj = 0; jj < 4; ++jj )
                {
                    const int z = y - 1 + jj;
                    double a0 = getpixel ( in , src_width , src_height , z , x , k );
                    double d0 = getpixel ( in , src_width , src_height , z , x - 1 , k ) - a0;
                    double d2 = getpixel ( in , src_width , src_height , z , x + 1 , k ) - a0;
                    double d3 = getpixel ( in , src_width , src_height , z , x + 2 , k ) - a0;
                    double a1 = -1.0 / 3 * d0 + d2 - 1.0 / 6 * d3;
                    double a2 = 1.0 / 2 * d0 + 1.0 / 2 * d2;
                    double a3 = -1.0 / 6 * d0 - 1.0 / 2 * d2 + 1.0 / 6 * d3;
                    C [ jj ] = a0 + a1 * dx + a2 * dx * dx + a3 * dx * dx * dx;

                    d0 = C [ 0 ] - C [ 1 ];
                    d2 = C [ 2 ] - C [ 1 ];
                    d3 = C [ 3 ] - C [ 1 ];
                    a0 = C [ 1 ];
                    a1 = -1.0 / 3 * d0 + d2 - 1.0 / 6 * d3;
                    a2 = 1.0 / 2 * d0 + 1.0 / 2 * d2;
                    a3 = -1.0 / 6 * d0 - 1.0 / 2 * d2 + 1.0 / 6 * d3;
                    double xfd = a0 + a1 * dy + a2 * dy * dy + a3 * dy * dy * dy;
                    xfd = ( xfd > 255 ) ? 255 : xfd;
                    xfd = ( xfd < 0 ) ? 0 : xfd;
                    out [ i * row_stride + j * channels + k ] = xfd;
                }
            }
        }
    }

    return out;*/
    return vector<unsigned char> ( );

/*
    std::vector<unsigned char> out ( dest_width * dest_height * 3 );

    float tx = float ( src_width ) / dest_width;
    float ty = float ( src_height ) / dest_height;
    int channels = 3;
    std::size_t row_stride = dest_width * channels;

    float C [ 5 ] = { 0 };

    for ( int i = 0; i < dest_height; ++i )
    {
        for ( int j = 0; j < dest_width; ++j )
        {
            int x = int ( tx * j );
            int y = int ( ty * i );
            float dx = tx * j - x;
            float dy = ty * i - y;

            for ( int k = 0; k < 3; ++k )
            {
                for ( int jj = 0; jj < 4; ++jj )
                {
                    int z = y - 1 + jj;
                    float a0 = getpixel ( in , src_width , src_height , z , x , k );
                    float d0 = getpixel ( in , src_width , src_height , z , x - 1 , k ) - a0;
                    float d2 = getpixel ( in , src_width , src_height , z , x + 1 , k ) - a0;
                    float d3 = getpixel ( in , src_width , src_height , z , x + 2 , k ) - a0;
                    float a1 = -1.0 / 3 * d0 + d2 - 1.0 / 6 * d3;
                    float a2 = 1.0 / 2 * d0 + 1.0 / 2 * d2;
                    float a3 = -1.0 / 6 * d0 - 1.0 / 2 * d2 + 1.0 / 6 * d3;
                    C [ jj ] = a0 + a1 * dx + a2 * dx * dx + a3 * dx * dx * dx;

                    d0 = C [ 0 ] - C [ 1 ];
                    d2 = C [ 2 ] - C [ 1 ];
                    d3 = C [ 3 ] - C [ 1 ];
                    a0 = C [ 1 ];
                    a1 = -1.0 / 3 * d0 + d2 - 1.0 / 6 * d3;
                    a2 = 1.0 / 2 * d0 + 1.0 / 2 * d2;
                    a3 = -1.0 / 6 * d0 - 1.0 / 2 * d2 + 1.0 / 6 * d3;
                    double value = a0 + a1 * dy + a2 * dy * dy + a3 * dy * dy * dy;
                    check_bounds ( value );
                    out [ i * row_stride + j * channels + k ] = value;
                }
            }
        }
    }

    return out;*/

}
vector<unsigned char> fromMatrix ( CMatrix2D<rgbD> &mtrx )
{
    vector<unsigned char> output;
    for ( auto &i : mtrx )
    {
        output.push_back ( i.Red );
        output.push_back ( i.Green );
        output.push_back ( i.Blue );
    }
    return output;
}
vector<unsigned char> fromImage ( BMP24 &img )
{
    vector<unsigned char> output;
    CMatrix2D<rgbD> pixdata = img.GetPixelData ( );
    //for ( size_t i = pixdata.size ( ) - 1; i > 0; i-- )
    //{
    //    output.push_back ( pixdata [ i ].Blue );
    //    output.push_back ( pixdata [ i ].Green );
    //    output.push_back ( pixdata [ i ].Red );
    //}
    for ( auto &i : pixdata )
    {
        output.push_back ( i.Red );
        output.push_back ( i.Green );
        output.push_back ( i.Blue );
    }
    return output;
}
CMatrix2D<rgbD> fromVector ( vector<unsigned char> &vec , size_t height , size_t width )
{
    CMatrix2D<rgbD> output ( height , width );
    for ( size_t i = 0; i < vec.size ( ); i += 3 )
    {
        output [ i / 3 ].Red = vec [ i ];
        output [ i / 3 ].Green = vec [ i + 1 ];
        output [ i / 3 ].Blue = vec [ i + 2 ];
    }
    return output;
}
