#include "BMP.h"
BMP8::BMP8 ( ) : CBMP ( )
{
}
BMP8::BMP8 ( const string &filename )
{
    this->LoadFile ( filename );
}
BMP8::BMP8 ( const BMP8 &obj )
{
    this->m_bHeadersLoad = obj.m_bHeadersLoad;
    this->m_Filename = obj.m_Filename;
    this->m_matData = obj.m_matData;
    this->m_sFileHeader = obj.m_sFileHeader;
    this->m_sInfoHeader = obj.m_sInfoHeader;
    this->m_Palette = obj.m_Palette;
}
BMP8::~BMP8 ( )
{
    m_matData.clear ( );
    m_Palette.clear ( );
#ifdef _DEBUG
    cout << "BMP8 Object has been destroyed.\n";
#endif
}
void BMP8::LoadFile ( const string &filename )
{
    m_Filename = filename;
    FilenameCheck ( string ( ".bmp" ) );
    LoadFile ( );
}
void BMP8::LoadFile ( )
{
    try
    {
        CBMP::LoadFile ( );
        if ( m_bHeadersLoad && m_fstFileInput.is_open ( ) )
        {
            if ( m_sFileHeader.bfType != 'MB' || ( m_Filesize < ( sizeof ( BITMAPINFOHEADER ) + sizeof ( BITMAPINFOHEADER ) ) ) )
            {
                this->~BMP8 ( );
            }
            if ( m_sInfoHeader.biBitCount == bit_depth_8 && m_sInfoHeader.biCompression == BI_RGB )
            {
                int palette_size = ( m_sFileHeader.bfOffBits - sizeof ( BITMAPFILEHEADER ) - sizeof ( BITMAPINFOHEADER ) ) / sizeof ( RGBQUAD );
                RGBQUAD *pPalette = new RGBQUAD [ palette_size ];
#ifdef _DEBUG
                cout << "Reading palette...\n";
#endif
                m_fstFileInput.read ( reinterpret_cast< char* >( pPalette ) , sizeof ( RGBQUAD )*palette_size );
                BYTE *pData = new BYTE [ m_sInfoHeader.biWidth*m_sInfoHeader.biHeight ];
                for ( auto i = 0; i < palette_size; i++ )
                {
                    m_Palette.push_back ( pPalette [ i ] );
                }
#ifdef _DEBUG
                cout << "Palette size : " << m_Palette.size ( ) << endl;
#endif
                m_matData.resize ( m_sInfoHeader.biHeight , m_sInfoHeader.biWidth );
#ifdef _DEBUG
                cout << "Reading data...\n";
#endif
                m_fstFileInput.read ( reinterpret_cast< char* >( pData ) , m_sInfoHeader.biWidth*m_sInfoHeader.biHeight );
                for ( size_t i = 0; i < m_matData.size ( ); i++ )
                {
                    m_matData [ i ] = pData [ i ];
                }
#ifdef _DEBUG
                cout << "Data size : " << m_matData.size ( ) << endl;
#endif
                delete [ ] pPalette;
                delete [ ] pData;
            }
            else
            {
                throw runtime_error ( "This is not 8 bit file.\n" );
            }
        }
        else
        {
            throw runtime_error ( "Can't open the file.\n" );
        }
    }
    catch ( runtime_error &err )
    {
        cout << err.what ( );
    }
}
void BMP8::SaveFile ( const string &filename )
{
    m_Filename = filename;
    FilenameCheck ( string ( ".bmp" ) );
    SaveFile ( );
}
void BMP8::SaveFile ( )
{
    try
    {
        if ( m_sInfoHeader.biBitCount == bit_depth_8 )
        {
            CBMP::SaveFile ( );
            if ( m_fstFileOutput.is_open ( ) && m_bHeadersLoad )
            {
                assert ( m_matData.size ( ) == ( m_sInfoHeader.biHeight*m_sInfoHeader.biWidth ) );
#ifdef _DEBUG
                cout << "Writing palette...\n";
#endif
                m_fstFileOutput.write ( reinterpret_cast< char* >( m_Palette.data ( ) ) , sizeof ( RGBQUAD )*m_Palette.size ( ) );
#ifdef _DEBUG
                cout << "Palette size : " << m_Palette.size ( ) << endl;
                cout << m_matData.size ( ) << endl;
#endif
                m_fstFileOutput.write ( reinterpret_cast< char* >( m_matData.get_all ( ).data ( ) ) , m_matData.size ( ) );
                m_fstFileOutput.close ( );
            }
        }
        else
        {
            throw runtime_error ( "This is not 8 bit image\n" );
        }
    }
    catch ( runtime_error &err )
    {
        cout << err.what ( );
    }
}
RGBTRIPLE BMP8::GetPixel ( unsigned y , unsigned x )
{
    assert ( ( x < static_cast<size_t>( m_sInfoHeader.biWidth ) ) && ( y < static_cast<size_t>( m_sInfoHeader.biHeight ) ) );
    return RGBTRIPLE
    {
        m_Palette [ ( int ) *m_matData.at ( m_sInfoHeader.biHeight - y - 1 ,x ) ].rgbBlue ,
        m_Palette [ ( int ) *m_matData.at ( m_sInfoHeader.biHeight - y - 1 ,x ) ].rgbGreen ,
        m_Palette [ ( int ) *m_matData.at ( m_sInfoHeader.biHeight - y - 1 ,x ) ].rgbRed
    };
}
void BMP8::ShowInfo ( )
{
    CBMP::ShowInfo ( );
    cout << "Palette info \n\tR\tG\tB\tReserved\n";
    for ( auto &i : m_Palette )
    {
        cout << "\t" << ( int ) i.rgbRed << "\t" << ( int ) i.rgbGreen << "\t" << ( int ) i.rgbBlue << "\t" << ( int ) i.rgbReserved << endl;
    }
}
CMatrix2D<rgbD> BMP8::GetPixelData ( ) const
{
    assert ( m_bHeadersLoad && m_matData.size ( ) > 0 );
    CMatrix2D<rgbD> temp ( m_sInfoHeader.biHeight , m_sInfoHeader.biWidth );
    assert ( m_matData.size ( ) == temp.size ( ) );
    for ( size_t i = 0; i < temp.size ( ); i++ )
    {
        temp [ i ] = rgbD (
            m_Palette [ ( int ) m_matData [ i ] ].rgbRed ,
            m_Palette [ ( int ) m_matData [ i ] ].rgbGreen ,
            m_Palette [ ( int ) m_matData [ i ] ].rgbBlue );
    }
    return temp;
}
CMatrix2D<int> BMP8::GetComponentPixmap ( int component ) const
{
    CMatrix2D<int> return_matrix ( m_sInfoHeader.biHeight , m_sInfoHeader.biWidth );
    switch ( component )
    {
        case Red:
            for ( auto i = 0; i < m_sInfoHeader.biHeight; i++ )
            {
                for ( auto j = 0; j < m_sInfoHeader.biWidth; j++ )
                {
                    return_matrix ( i , j ) = m_Palette [ ( int ) m_matData ( i , j ) ].rgbRed;
                }
            }
            break;
        case Green:
            for ( auto i = 0; i < m_sInfoHeader.biHeight; i++ )
            {
                for ( auto j = 0; j < m_sInfoHeader.biWidth; j++ )
                {
                    return_matrix ( i , j ) = m_Palette [ ( int ) m_matData ( i , j ) ].rgbGreen;
                }
            }
            break;
        case  Blue:
            for ( auto i = 0; i < m_sInfoHeader.biHeight; i++ )
            {
                for ( auto j = 0; j < m_sInfoHeader.biWidth; j++ )
                {
                    return_matrix ( i , j ) = m_Palette [ ( int ) m_matData ( i , j ) ].rgbBlue;
                }
            }
            break;
        default:
            break;
    }
    return return_matrix;
}
CMatrix2D<float> BMP8::GetComponentData ( int component )const
{                               // stub
    return CMatrix2D<float> ( );
}