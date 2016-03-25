#include "BMP.h"
BMP24::BMP24 ( ) : CBMP ( )
{
    BITMAPFILEHEADER BMFH;
    BITMAPINFOHEADER BMIH;
    BMFH.bfOffBits = 54;
    BMFH.bfReserved1 = 0;
    BMFH.bfReserved2 = 0;
    BMFH.bfSize = 54;
    BMFH.bfType = 19778;
    BMIH.biBitCount = 24;
    BMIH.biClrImportant = 0;
    BMIH.biClrUsed = 0;
    BMIH.biSize = 40;
    BMIH.biXPelsPerMeter = 0;
    BMIH.biYPelsPerMeter = 0;
    BMIH.biPlanes = 1;
    BMIH.biSizeImage = 0;
    BMIH.biCompression = 0;
    BMIH.biHeight = 0;
    BMIH.biWidth = 0;
    this->SetInfoHeader ( BMIH );
    this->SetFileHeader ( BMFH );
    m_bHeadersLoad = true;

}
BMP24::BMP24 ( const BMP24 &obj )
{
    this->m_bHeadersLoad = obj.m_bHeadersLoad;
    this->m_Filename = obj.m_Filename;
    this->m_matData = obj.m_matData;
    this->m_sFileHeader = obj.m_sFileHeader;
    this->m_sInfoHeader = obj.m_sInfoHeader;
}
BMP24::BMP24 ( const string &filename )
{
    this->LoadFile ( filename );
}
BMP24::~BMP24 ( )
{
    m_matData.clear ( );
#ifdef _DEBUG
    cout << m_Filename << " - BMP24 Object has been destroyed.\n";
#endif
}
void BMP24::LoadFile ( const string &filename )
{
    m_Filename = filename;
    FilenameCheck ( string ( ".bmp" ) );
    LoadFile ( );
}
bool BMP24::isEmpty ( )
{
    return m_bLoadedData;
}
void BMP24::LoadFile ( )
{
    try
    {
        CBMP::LoadFile ( );
        if ( m_fstFileInput.is_open ( ) && m_bHeadersLoad )
        {
            if ( m_sFileHeader.bfType != 'MB' || ( m_Filesize < ( sizeof ( BITMAPINFOHEADER ) + sizeof ( BITMAPINFOHEADER ) ) ) )
            {
                this->~BMP24 ( );
            }
            if ( m_sInfoHeader.biBitCount == bit_depth_24 && m_sInfoHeader.biCompression == BI_RGB )
            {
                m_matData.resize ( m_sInfoHeader.biHeight , m_sInfoHeader.biWidth );
                auto nStringLength = linewidth_calc ( m_sInfoHeader.biWidth );
                m_fstFileInput.seekg ( m_fstFileInput.beg + m_sFileHeader.bfOffBits );
                RGBTRIPLE *pStringBuffer = new RGBTRIPLE [ nStringLength ];
                for ( auto i = 0; i < m_sInfoHeader.biHeight; i++ )
                {
                    m_fstFileInput.read ( reinterpret_cast< char* >( pStringBuffer ) , nStringLength );
                    for ( auto j = 0; j < m_sInfoHeader.biWidth; j++ )
                    {
                        m_matData.set ( m_matData.GetHeight() - i - 1 , j , static_cast< rgbD >( pStringBuffer [ j ] ) ); // fixed
                    }
                }
#ifdef _DEBUG
                cout << m_Filename << " - Reading data...\n";
#endif
                delete [ ] pStringBuffer;
            }
            else
            {
                throw runtime_error ( "This is not 24 bit file\n" );
            }
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
CMatrix2D<int> BMP24::GetComponentPixmap ( int component ) const
{
    CMatrix2D<int> return_matrix ( m_sInfoHeader.biHeight , m_sInfoHeader.biWidth );
    CMatrix2D<int> buffer;
    switch ( component )
    {
        case Red:
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = m_matData [ i ].Red;
            }
            break;
        case Green:
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = m_matData [ i ].Green;
            }
            break;
        case  Blue:
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = m_matData [ i ].Blue;
            }
            break;
        case Y:
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = 0.299 * m_matData [ i ].Red + 0.587 * m_matData [ i ].Green + 0.114 * m_matData [ i ].Blue;
            }
            break;
        case Cb:
            buffer = GetComponentPixmap ( Y );
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = 0.5643 * ( m_matData [ i ].Blue - buffer [ i ] ) + 128;
            }
            break;
        case Cr:
            buffer = GetComponentPixmap ( Y );
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = 0.7132 * ( m_matData [ i ].Red - buffer [ i ] ) + 128;
            }
            break;
        default:
            break;
    }
    return return_matrix;
}
CMatrix2D<float> BMP24::GetComponentData ( int component ) const
{
    CMatrix2D<float> return_matrix ( m_sInfoHeader.biHeight , m_sInfoHeader.biWidth );
    CMatrix2D<float> buffer;
    switch ( component )
    {
        case Red:
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = m_matData [ i ].Red;
            }
            break;
        case Green:
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = m_matData [ i ].Green;
            }
            break;
        case  Blue:
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = m_matData [ i ].Blue;
            }
            break;
        case Y:
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = 0.299 * m_matData [ i ].Red + 0.587 * m_matData [ i ].Green + 0.114 * m_matData [ i ].Blue;
            }
            break;
        case Cb:
            buffer = GetComponentData ( Y );
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = 0.5643 * ( m_matData [ i ].Blue - buffer [ i ] ) + 128;
            }
            break;
        case Cr:
            buffer = GetComponentData ( Y );
            for ( size_t i = 0; i < m_matData.size ( ); i++ )
            {
                return_matrix [ i ] = 0.7132 * ( m_matData [ i ].Red - buffer [ i ] ) + 128;
            }
            break;
        default:
            break;
    }
    return return_matrix;
}
void BMP24::SaveFile ( const string &filename )
{
    m_Filename = filename;
    FilenameCheck ( string ( ".bmp" ) );
    SaveFile ( );
}
void BMP24::SaveFile ( )
{
    try
    {
        if ( m_sInfoHeader.biBitCount == bit_depth_24 )
        {
            CBMP::SaveFile ( );
            if ( m_fstFileOutput.is_open ( ) && m_bHeadersLoad )
            {
                m_fstFileInput.seekg ( m_fstFileOutput.beg + m_sFileHeader.bfOffBits );
                auto nStringLength = linewidth_calc ( m_sInfoHeader.biWidth );
                RGBTRIPLE *pStringBuffer = new RGBTRIPLE [ nStringLength ];
#ifdef _DEBUG
                cout << m_Filename << " - Writing data...\n";
#endif
                for ( auto i = m_sInfoHeader.biHeight - 1; i >= 0; i-- )
                {
                    //vector<rgbD> temp_vector = m_matData.get_row ( i );
                    //for ( auto j = 0; j < temp_vector.size ( ); j++ )
                    //{
                    //	pStringBuffer[ j ] = static_cast< RGBTRIPLE >( temp_vector[ j ] );
                    //}
                    //m_fstFileOutput.write ( reinterpret_cast< char* >( pStringBuffer ) ,nStringLength );
                    for ( size_t j = 0; j < m_matData.GetWidth ( ); j++ )
                    {
                        pStringBuffer [ j ] = static_cast< RGBTRIPLE >( m_matData ( i , j ) );
                    }
                    m_fstFileOutput.write ( reinterpret_cast< char* >( pStringBuffer ) , nStringLength );
                }
                m_fstFileOutput.close ( );
                delete [ ] pStringBuffer;
            }
        }
        else
        {
            throw runtime_error ( "This is not 24 bit image\n" );
        }
    }
    catch ( runtime_error &err )
    {
        cout << err.what ( );
    }
}
CMatrix2D<rgbD> BMP24::GetPixelData ( ) const
{
    return m_matData;
}
void BMP24::ShowInfo ( )
{
    CBMP::ShowInfo ( );
}
void BMP24::Resize ( unsigned Height , unsigned Width )
{
    m_matData.resize ( Height , Width );
    m_sInfoHeader.biHeight = Height;
    m_sInfoHeader.biWidth = Width;

}
void BMP24::recompute_headers ( )
{
    m_sFileHeader.bfOffBits = sizeof ( BITMAPINFOHEADER ) + sizeof ( BITMAPFILEHEADER );
    m_sInfoHeader.biSizeImage = 0;
}
RGBTRIPLE BMP24::GetPixel ( unsigned y , unsigned x )
{
    assert ( ( x < m_sInfoHeader.biWidth ) && ( y < m_sInfoHeader.biHeight ) );
    return RGBTRIPLE
    {
        static_cast< BYTE >( m_matData.at ( m_sInfoHeader.biHeight - y - 1 ,x )->Blue ) ,
        static_cast< BYTE >( m_matData.at ( m_sInfoHeader.biHeight - y - 1 ,x )->Green ) ,
        static_cast< BYTE >( m_matData.at ( m_sInfoHeader.biHeight - y - 1 ,x )->Red )
    };
}
void BMP24::SetPixelData ( CMatrix2D<rgbD> &obj )
{
    m_matData = obj;
    m_sInfoHeader.biHeight = obj.GetHeight ( );
    m_sInfoHeader.biWidth = obj.GetWidth ( );
}
void BMP24::SplitIntoColors ( BMP24 &src )
{
    vector<string> str;
    str.push_back ( Red_Postfix );
    str.push_back ( Green_Postfix );
    str.push_back ( Blue_Postfix );
    vector<CMatrix2D<int>> rgb;
    for ( auto i = 0; i < 3; i++ )
    {
        BMP24 *pic = new BMP24 ( src );
        CMatrix2D<rgbD> pixelmap = pic->GetPixelData ( );
        switch ( i )
        {
            case Red:
                for ( auto &i : pixelmap )
                {
                    i.destroy_component ( Red );
                }
                rgb.push_back ( pic->GetComponentPixmap ( Red ) );
                break;
            case Green:
                for ( auto &i : pixelmap )
                {
                    i.destroy_component ( Green );
                }
                rgb.push_back ( pic->GetComponentPixmap ( Green ) );
                break;
            case Blue:
                for ( auto &i : pixelmap )
                {
                    i.destroy_component ( Blue );
                }
                rgb.push_back ( pic->GetComponentPixmap ( Blue ) );
                break;
            default:
                break;
        }
        pic->SetPixelData ( pixelmap );
        pic->AddToFilename ( str [ i ] );
        pic->SaveFile ( );
        delete pic;
    }
    cout << src.GetFilename ( ) + " R - G (Correlation): " << Correlation ( rgb [ 0 ] , rgb [ 1 ] ) << endl;
    cout << src.GetFilename ( ) + " G - B (Correlation): " << Correlation ( rgb [ 1 ] , rgb [ 2 ] ) << endl;
    cout << src.GetFilename ( ) + " B - R (Correlation): " << Correlation ( rgb [ 2 ] , rgb [ 0 ] ) << endl;
}
void BMP24::BuildHistogramm ( const BMP24 &src )
{
    vector<string> a;
    a.push_back ( Red_Postfix );
    a.push_back ( Green_Postfix );
    a.push_back ( Blue_Postfix );
    for ( auto i = 0; i < 3; i++ )
    {
        BMP24::BuildHistogramm ( src.GetFilename ( ) + a [ i ] , src.GetComponentPixmap ( i ) );
    }
}
CMatrix2D<rgbD> *BMP24::GetInternal ( )
{
    return &m_matData;
}
void BMP24::BuildHistogramm ( const string &filename , const CMatrix2D<int> &obj )
{
    fstream OutputStream ( filename + string ( ".txt" ) , ios::out );
    vector<int> output ( 256 , 0 );
    for ( auto &i : obj )
    {
        output [ i ]++;
    }
    for ( auto &i : output )
    {
        OutputStream << i << "\t";
    }
    OutputStream.close ( );
}
void BMP24::ConvertToYCbCr ( BMP24 &src , unsigned ratio , bool type )
{
    assert ( ratio >= 0 );
    CMatrix2D<rgbD> Y ( src.GetPixelData ( ) );
    CMatrix2D<rgbD> Cb ( src.GetPixelData ( ) );
    CMatrix2D<rgbD> Cr ( src.GetPixelData ( ) );
    CMatrix2D<rgbD> PixelMap ( src.GetPixelData ( ) );
    BMP24 Y_color ( src );
    BMP24 Cb_color ( src );
    BMP24 Cr_color ( src );
    for ( auto i = 0; i < Y.size ( ); i++ )
    {
        Y [ i ] = 0.299 * PixelMap [ i ].Red + 0.587 * PixelMap [ i ].Green + 0.114 * PixelMap [ i ].Blue;
        Cb [ i ] = 0.5643 * ( PixelMap [ i ].Blue - Y [ i ].Red ) + 128;
        Cr [ i ] = 0.7132 * ( PixelMap [ i ].Red - Y [ i ].Red ) + 128;
    }
    Y_color.AddToFilename ( const_cast<string &>( Y_Postfix ) );
    Cb_color.AddToFilename ( const_cast<string &>( Cb_Postfix ) );
    Cr_color.AddToFilename ( const_cast<string &>( Cr_Postfix ) );
    if ( ratio != None )
    {
        switch ( ratio )
        {
            case Double:
                Cb.decimation ( Double , type );
                Cr.decimation ( Double , type );
                Cb_color.AddToFilename ( string ( "_2X" ) );
                Cr_color.AddToFilename ( string ( "_2X" ) );
                break;
            case Quad:
                Cb.decimation ( Quad , type );
                Cr.decimation ( Quad , type );
                Cb_color.AddToFilename ( string ( "_4X" ) );
                Cr_color.AddToFilename ( string ( "_4X" ) );
                break;
            default:
                break;
        }
        BITMAPINFOHEADER temp_infoheader = Cb_color.GetInfoHeader ( );
        temp_infoheader.biWidth = Cb.get_size ( ).m_Second;
        temp_infoheader.biHeight = Cb.get_size ( ).m_First;
        Cb_color.SetInfoHeader ( temp_infoheader );
        Cr_color.SetInfoHeader ( temp_infoheader );
    }
    Y_color.SetPixelData ( Y );
    Cb_color.SetPixelData ( Cb );
    Cr_color.SetPixelData ( Cr );
    BuildHistogramm ( Cb_color.GetFilename ( ) + ".txt" , Cb_color.GetComponentPixmap ( Red ) );
    BuildHistogramm ( Cr_color.GetFilename ( ) + ".txt" , Cr_color.GetComponentPixmap ( Red ) );
    Y_color.SaveFile ( );
    Cb_color.SaveFile ( );
    Cr_color.SaveFile ( );
}
BMP24 BMP24::ConvertToRGB ( BMP24 &Y_pic , BMP24 &Cb_pic , BMP24 &Cr_pic )
{
    BMP24 temp ( Y_pic );
    int Y_Axis_ratio = ( double ) Y_pic.GetHeight ( ) / ( double ) Cb_pic.GetHeight ( );
    int X_Axis_ratio = ( double ) Y_pic.GetWidth ( ) / ( double ) Cb_pic.GetWidth ( );
    CMatrix2D<rgbD> Y = Y_pic.GetPixelData ( );
    CMatrix2D<rgbD> Cb = Cb_pic.GetPixelData ( );
    CMatrix2D<rgbD> Cr = Cr_pic.GetPixelData ( );
    CMatrix2D<rgbD> RGB = Y_pic.GetPixelData ( );
    if ( Y_Axis_ratio > 1 && X_Axis_ratio > 1 )
    {
        switch ( Y_Axis_ratio )
        {
            case Double:
                Cb.upscaling ( Double );
                Cr.upscaling ( Double );
                temp.AddToFilename ( string ( "_From_2X" ) );
                break;
            case Quad:
                Cb.upscaling ( Quad );
                Cr.upscaling ( Quad );
                temp.AddToFilename ( string ( "_From_4X" ) );
                break;
            default:
                break;
        }
    }
    if ( Cb.get_size ( ).m_First < Y.get_size ( ).m_First || Cb.get_size ( ).m_First < Y.get_size ( ).m_Second ) // Fixing Y - CbCr maps size mismatch
    {
        int y = Y.get_size ( ).m_First - Cb.get_size ( ).m_First;
        int x = Y.get_size ( ).m_Second - Cb.get_size ( ).m_Second;
        Cb.resize ( Y.get_size ( ).m_First , Y.get_size ( ).m_Second );
        Cr.resize ( Y.get_size ( ).m_First , Y.get_size ( ).m_Second );
        for ( auto i = Y.get_size ( ).m_First - y - 1; i < Y.get_size ( ).m_First; i++ )
        {
            Cb.replace_row ( Cb.get_row ( Y.get_size ( ).m_First - y - 2 ) , i );
            Cr.replace_row ( Cr.get_row ( Y.get_size ( ).m_First - y - 2 ) , i );
        }
        for ( auto i = Y.get_size ( ).m_Second - x - 1; i < Y.get_size ( ).m_Second; i++ )
        {
            Cb.replace_column ( Cb.get_column ( Y.get_size ( ).m_Second - x - 2 ) , i );
            Cr.replace_column ( Cr.get_column ( Y.get_size ( ).m_Second - x - 2 ) , i );
        }
    }
    for ( auto i = 0; i < Y.size ( ); i++ )
    {
        RGB [ i ].Green = Y [ i ].Blue - 0.714 * ( Cr [ i ].Blue - 128 ) - 0.334 * ( Cb [ i ].Blue - 128 );
        RGB [ i ].Red = Y [ i ].Blue + 1.402 * ( Cr [ i ].Blue - 128 );
        RGB [ i ].Blue = Y [ i ].Blue + 1.772 * ( Cb [ i ].Blue - 128 );
        check_bounds ( RGB [ i ].Red );
        check_bounds ( RGB [ i ].Green );
        check_bounds ( RGB [ i ].Blue );
    }
    temp.SetPixelData ( RGB );
    temp.AddToFilename ( const_cast<string &>( Restored_Postfix ) );
    return temp;
}
void BMP24::SetComponentData ( CMatrix2D<int> &obj , int Component )
{
    assert ( ( obj.GetHeight ( ) == this->GetHeight ( ) ) && ( obj.GetWidth ( ) == this->GetWidth ( ) ) );
    switch ( Component )
    {
        case Red:
            for ( auto i = 0; i < this->m_matData.size ( ); i++ )
            {
                m_matData [ i ].Red = obj [ i ];

            }
            break;
        case Green:
            for ( auto i = 0; i < this->m_matData.size ( ); i++ )
            {
                m_matData [ i ].Green = obj [ i ];
            }
            break;
        case Blue:
            for ( auto i = 0; i < this->m_matData.size ( ); i++ )
            {
                m_matData [ i ].Blue = obj [ i ];
            }
            break;
        default:
            break;
    }
}