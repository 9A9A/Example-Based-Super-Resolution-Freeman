#ifndef _UTILITIES_H_
#define _UTILITIES_H_
#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <functional>
#include <fstream>
#include <assert.h>
#include <string>
#include <Windows.h>
#include <chrono>
#include <map>
#include <unordered_map>
#pragma warning(disable:4068)
#pragma warning(disable:4018)
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4305)
using namespace std;
using namespace chrono;
//#define template<typename _Ty> template<typename _Ty>
#define abstract_class class
const double PI2 = 2 * 3.1415926535897932384;
const double PI = 3.1415926535897932384;
const double EXP = 2.7182818284590452353602874;
const static size_t thread_amount = thread::hardware_concurrency ( );
enum bit_depth
{
    bit_depth_1 = 1 ,
    bit_depth_2 = 2 ,
    bit_depth_4 = 4 ,
    bit_depth_8 = 8 ,
    bit_depth_16 = 16 ,
    bit_depth_24 = 24 ,
    bit_depth_32 = 32 ,
    bit_depth_64 = 64 ,
    bit_depth_128 = 128
};
enum Color
{
    Red = 0 ,
    Green = 1 ,
    Blue = 2 ,
    Y = 3 ,
    Cb = 4 ,
    Cr = 5
};
enum DPCM_Mode
{
    Left = 0 ,
    Top = 1 ,
    TopLeft = 2 ,
    Average_Neib = 3
};
enum Decimation_ratio
{
    None = 0 ,
    Double = 2 ,
    Quad = 4
};

enum Decimation_Type_
{
    Direct_point = 0 ,
    Average_value = 1
};
inline void check_bounds ( short &value )
{
    ( value > 255 ) ? value = 255 : value;
    ( value < 0 ) ? value = 0 : value;
}
inline void check_bounds ( int &value )
{
    ( value > 255 ) ? value = 255 : value;
    ( value < 0 ) ? value = 0 : value;
}
inline void check_bounds ( double &value )
{
    ( value > 255.0 ) ? value = 255.0 : value;
    ( value < 0.0 ) ? value = 0.0 : value;
}
inline void check_bounds ( float &value )
{
    ( value > 255.0f ) ? value = 255.0f : value;
    ( value < 0.0f ) ? value = 0.0f : value;
}
inline void check_bounds ( byte &value )
{
    ( value > 255 ) ? value = 255 : value;
    ( value < 0 ) ? value = 0 : value;
}

struct rgbD;
inline void check_bounds ( rgbD &value );
struct rgbD
{
    double Red;
    double Green;
    double Blue;
    explicit rgbD ( const RGBTRIPLE &obj ) : Red ( obj.rgbtRed ) , Green ( obj.rgbtGreen ) , Blue ( obj.rgbtBlue )
    {
    };
    rgbD ( double R , double G , double B ) : Red ( R ) , Green ( G ) , Blue ( B )
    {
    };
    rgbD ( ) : Red ( 0 ) , Green ( 0 ) , Blue ( 0 )
    {
    };
    void destroy_component ( int component )
    {
        switch ( component )
        {
            case 0:
                Blue = 0;
                Green = 0;
                break;
            case 1:
                Red = 0;
                Blue = 0;
                break;
            case 2:
                Red = 0;
                Green = 0;
                break;
            default:
                break;
        }
    }
    rgbD &operator = ( double in )
    {
        Red = static_cast< double >( in );
        Blue = static_cast< double >( in );
        Green = static_cast< double >( in );
        return *this;
    }
    rgbD &operator = ( const rgbD& lhs )
    {
        this->Red = lhs.Red;
        this->Green = lhs.Green;
        this->Blue = lhs.Blue;
        return *this;
    }
    float operator []( size_t index )
    {
        switch ( index )
        {
            case Color::Red: return Red;
            case Color::Green: return Green;
            case Color::Blue: return Blue;
            default:
                throw runtime_error ( "rgbD struct : this is wrong index!" );
        }
    }
    operator RGBTRIPLE( )
    {
        return RGBTRIPLE { static_cast<BYTE>( Blue ) ,static_cast<BYTE>( Green ) ,static_cast<BYTE>( Red ) };
    }
    friend rgbD operator + ( const rgbD &lhs , const rgbD &rhs )
    {
        rgbD x;
        x.Red = lhs.Red + rhs.Red; x.Green = lhs.Green + rhs.Green; x.Blue = lhs.Blue + rhs.Blue;
        //check_bounds ( x );
        return x;
    }
    friend rgbD operator - ( const rgbD &lhs , const rgbD &rhs )
    {
        rgbD x;
        x.Red = lhs.Red - rhs.Red; x.Green = lhs.Green - rhs.Green; x.Blue = lhs.Blue - rhs.Blue;
        //check_bounds ( x );
        return x;
    }
    friend rgbD operator / ( const rgbD &lhs , const int rhs )
    {
        return rgbD ( lhs.Red / rhs , lhs.Green / rhs , lhs.Blue / rhs );
    }
    friend ostream &operator << ( ostream &os , const rgbD &obj )
    {
        os << "(" << obj.Red << "," << obj.Green << "," << obj.Blue << ")";
        return os;
    }
    friend bool operator != ( const rgbD &lhs , const rgbD &rhs )
    {
        return ( ( lhs.Blue != rhs.Blue ) || ( lhs.Green != rhs.Green ) || ( lhs.Red != rhs.Red ) ) ? true : false;
    }
    friend bool operator == ( const rgbD &lhs , const rgbD &rhs )
    {
        return ( !( lhs != rhs ) ) ? true : false;
    }
    bool operator == ( const double value )
    {
        return( ( value == Red ) && ( value == Green ) && ( value == Blue ) ) ? true : false;
    }
};
inline void check_bounds ( rgbD &value )
{
    check_bounds ( value.Red );
    check_bounds ( value.Green );
    check_bounds ( value.Blue );
}

template<typename _Ty> _Ty AVG ( vector<_Ty> &list )
{
    _Ty temp ( );
    for ( auto &i : list )
    {
        temp += i;
    }
    return temp;
}
template<typename _Ty> double L_Norm ( vector<_Ty> &vec , size_t norm_factor )
{
    double summary = 0;
    for ( auto &i : vec )
    {
        summary += pow ( i , norm_factor );
    }
    return abs ( pow ( summary , 1.0 / norm_factor ) );
}
struct rgbA
{
    double Red;
    double Green;
    double Blue;
    explicit rgbA ( const RGBTRIPLE &obj )
    {
        *this = static_cast<rgbA>( static_cast< rgbD >( obj ) );
    };
    rgbA ( const rgbD &obj ) : Red ( static_cast< double >( obj.Red ) / 256 ) , Green ( static_cast< double >( obj.Green ) / 256 ) , Blue ( static_cast< double >( obj.Blue ) / 256 )
    {
    };
    rgbA ( double A , double B , double C ) : Red ( A ) , Green ( B ) , Blue ( C )
    {
    };
    rgbA ( ) : Red ( 0 ) , Green ( 0 ) , Blue ( 0 )
    {
    };
    friend ostream &operator << ( ostream &os , const rgbA &obj )
    {
        os << "(" << obj.Red << "," << obj.Green << "," << obj.Blue << ")";
        return os;
    }
};
inline int linewidth_calc ( int width )
{
    return ( ( width*sizeof ( RGBTRIPLE ) ) % 4 == 0 ) ? ( width*sizeof ( RGBTRIPLE ) ) : ( ( 4 - ( ( width*sizeof ( RGBTRIPLE ) ) % 4 ) ) + width*sizeof ( RGBTRIPLE ) );
}
template<typename _Ty> struct point
{
    _Ty m_First;
    _Ty m_Second;
};
//template<typename _Ty> struct mtrxinit
//{
//	initializer_list<_Ty> mtrx_initlist;
//};
template<typename _Ty> class CMatrix2D // basic matrix and operations
{
private:
    _Ty abs ( _Ty );
    unsigned m_uRowCount;
    unsigned m_uColumnCount;
    vector<_Ty> m_data;
    void swap ( CMatrix2D<_Ty> &src );
public:

    CMatrix2D<_Ty> ( ); // default 0 element matrix
    CMatrix2D<_Ty> ( const CMatrix2D<_Ty> & ); // copy constructor
    CMatrix2D<_Ty> ( CMatrix2D<_Ty> && ); // move constructor
    CMatrix2D<_Ty> ( unsigned Rows , unsigned Column ); // Row x Column 0 valued matrix
    CMatrix2D<_Ty> ( unsigned Rows , unsigned Column , _Ty value ); // Row x Column matrix with all values = Value;
    CMatrix2D<_Ty> ( initializer_list<_Ty> list ); // build one-line matrix
    CMatrix2D<_Ty> ( vector<initializer_list<_Ty>> vals );
    CMatrix2D<_Ty> ( _Ty *ptr , size_t h , size_t w );
    //CMatrix2D<_Ty> ( initializer_list<mtrxinit<_Ty>> list );
    CMatrix2D<_Ty> ( vector<_Ty> &vec ); // build one-line matrix
    CMatrix2D<_Ty> ( vector<_Ty> &vec , unsigned Rows , unsigned Column );
    CMatrix2D<_Ty> ( _Ty value ); // 1x1 matrix;
    ~CMatrix2D<_Ty> ( );


    //basic functions
    auto begin ( ) -> decltype( m_data.begin ( ) )
    {
        return m_data.begin ( );
    }
    auto end ( ) -> decltype( m_data.end ( ) )
    {
        return m_data.end ( );
    }
    auto begin ( ) const -> decltype( m_data.begin ( ) )
    {
        return m_data.begin ( );
    }
    auto end ( ) const -> decltype( m_data.end ( ) )
    {
        return m_data.end ( );
    }
    auto operator[]( int i ) -> decltype( m_data [ i ] )
    {
        if ( i < m_data.size ( ) )
        {
            return m_data [ i ];
        }
        else
        {
            throw runtime_error ( to_string ( i ) + " of " + to_string ( m_data.size ( ) ) + " out of range\n" );
        }
    }
    _Ty operator[]( int i ) const
    {
        if ( i < m_data.size ( ) )
        {
            return m_data [ i ];
        }
        else
        {
            throw runtime_error ( to_string ( i ) + " of " + to_string ( m_data.size ( ) ) + " out of range\n" );
        }
    }
    void for_each ( function<void ( int & )> f )
    {
        for ( auto &i : m_data )
        {
            f ( i );
        }
    }
    bool isSquare ( ) const; // if matrix size = n x n returns true, else false;
    void clear ( );	 // clear matrix
    void fill ( _Ty value ); // fill matrix with value
    void fill_rnd ( );
    void set ( unsigned row , unsigned column , _Ty value ); // set value
    void resize ( unsigned new_row , unsigned new_column ); // resize matrix
    void remove_row ( unsigned row ); // remove row
    void remove_column ( unsigned column ); // remove_column
    void insert_row ( const vector<_Ty> &vec , unsigned row ); // adding new row to matirx (increase size matrix by 1)
    void insert_column ( const vector<_Ty> &vec , unsigned column ); // adding new column to matrix (increase size matrix by 1)
    void replace_row ( const vector<_Ty> &vec , unsigned row );
    void replace_column ( const vector<_Ty> &vec , unsigned column );
    void reverse_row ( unsigned row );
    void reverse_rows_all ( );
    void reverse_column ( unsigned column );
    void reverse_column_all ( );
    void transpose ( );
    void normalize ( );
    void decimate ( unsigned ratio , unsigned start_pos );
    void decimation ( unsigned ratio , bool type );
    void upscaling ( unsigned ratio );
    void y_axis_shift ( int y );
    void x_axis_shift ( int x );
    void emplace ( CMatrix2D<_Ty>& mtrx , size_t y , size_t x );
    void right_append ( CMatrix2D<_Ty>& mtrx , _Ty _value );
    vector<_Ty> &data ( );
    static _Ty sum ( CMatrix2D<_Ty> & obj );
    void DPCM ( unsigned Mode );
    vector<_Ty> get_row ( unsigned row );
    vector<_Ty> get_column ( unsigned column );
    vector<_Ty> get_all ( ) const;
    _Ty *get_max ( );
    point<_Ty> get_max_pos ( );
    _Ty *get_min ( );
    point<_Ty> get_min_pos ( );
    _Ty *at ( unsigned row , unsigned column );
    point<unsigned> get_size ( ) const;
    unsigned size ( ) const;
    void get_memory_size ( )const ;
    //static functions
    static CMatrix2D<_Ty> Hadamard ( const CMatrix2D<_Ty> &lhs , const CMatrix2D<_Ty> &rhs );
    unsigned GetHeight ( )
    {
        return m_uRowCount;
    }
    unsigned GetWidth ( )
    {
        return m_uColumnCount;
    }
    CMatrix2D<_Ty> GetSquare ( unsigned y , unsigned x , unsigned radius );
    CMatrix2D<_Ty*> sqr ( size_t y , size_t x , size_t tall , size_t wide );
    CMatrix2D<_Ty> rect ( size_t y , size_t x , size_t tall , size_t wide );
    //operators
    CMatrix2D<_Ty> & operator = ( const CMatrix2D<_Ty> & );
    CMatrix2D<_Ty> & operator = ( CMatrix2D<_Ty> && );
    template<typename _Ty> friend ostream & operator << ( ostream & , const CMatrix2D<_Ty> & );
    template<typename _Ty> friend CMatrix2D<_Ty> operator + ( const CMatrix2D<_Ty> & , const _Ty );
    template<typename _Ty> friend CMatrix2D<_Ty> operator + ( const CMatrix2D<_Ty> & , const CMatrix2D<_Ty> & );
    template<typename _Ty> friend CMatrix2D<_Ty> operator - ( const CMatrix2D<_Ty> & , const _Ty );
    template<typename _Ty> friend CMatrix2D<_Ty> operator - ( const CMatrix2D<_Ty> & , const CMatrix2D<_Ty> & );
    template<typename _Ty> friend CMatrix2D<_Ty> operator * ( const CMatrix2D<_Ty> & , _Ty );
    template<typename _Ty> friend CMatrix2D<_Ty> operator * ( const CMatrix2D<_Ty> & , const CMatrix2D<_Ty> & );
    template<typename _Ty> friend CMatrix2D<_Ty> operator / ( const CMatrix2D<_Ty> & , const _Ty );
    //template<typename _T1 , typename _T2> friend bool operator == ( const CMatrix2D<_T1> &lhs , const CMatrix2D<_T2> &rhs );
    //template<typename _T1 , typename _T2> friend bool operator != ( const CMatrix2D<_T1> &lhs , const CMatrix2D<_T2> &rhs );
    ////bool operator != ( const CMatrix2D<_Ty> &other ) const;

    //template<typename _Ty2 ,typename _Ty> operator CMatrix2D<_Ty2> ( );
    _Ty& operator ()( unsigned , unsigned );
    _Ty operator ()( unsigned , unsigned ) const;
    operator vector<_Ty> ( );// vector<_Ty> cast operator
                             //_Type_ friend CMatrix2D<_Ty> operator / ( const CMatrix2D<_Ty> & ,const CMatrix2D<_Ty> & ); it's not implemented yet TO DO : calculating determinant func
                             //inline functions
    bool bSizeEqual ( const CMatrix2D<_Ty> & , const CMatrix2D<_Ty> & );
    bool bSize ( unsigned row , unsigned column );
};
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( )
{
    m_uRowCount = 0;
    m_uColumnCount = 0;
}
inline float* arr ( CMatrix2D<rgbD>& value )
{
    auto ret = new float [ 176 ];
    memset ( ret , 0 , sizeof ( float ) * 176 );
    size_t pos = 0;
    for ( auto &i : value )
    {
        ret [ pos++ ] = i.Red;
        ret [ pos++ ] = i.Green;
        ret [ pos++ ] = i.Blue;
    }
    return ret;
}
//template<typename _Type_name1 , typename _Type_name2> CMatrix2D<_Type_name1> matrix_cast ( CMatrix2D<_Type_name2> &obj )
//{
//    CMatrix2D<_Type_name1> temp ( obj.GetHeight ( ) , obj.GetWidth ( ) );
//    for ( auto i = 0; i < obj.size ( ); i++ )
//    {
//        temp [ i ] = static_cast< _Type_name1 >( obj [ i ] );
//    }
//    return temp;
//}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( const CMatrix2D<_Ty> &obj )
{
    m_uRowCount = obj.m_uRowCount;
    m_uColumnCount = obj.m_uColumnCount;
    m_data = obj.m_data;
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( unsigned Rows , unsigned Column ) : m_uRowCount ( Rows ) , m_uColumnCount ( Column )
{
    vector<_Ty> tmp ( Rows*Column );
    m_data = tmp;
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( unsigned Rows , unsigned Column , _Ty value ) : m_uRowCount ( Rows ) , m_uColumnCount ( Column )
{
    vector<_Ty> tmp ( Rows*Column , value );
    m_data = tmp;
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( _Ty value ) : m_uRowCount ( 1 ) , m_uColumnCount ( 1 )
{
    m_data.push_back ( value );
}
template<typename _Ty> CMatrix2D<_Ty>::operator std::vector<_Ty , std::allocator<_Ty>> ( )
{
    return m_data;
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( initializer_list<_Ty> list )
{
    for ( auto &i : list )
    {
        m_data.push_back ( i );
    }
    m_uColumnCount = m_data.size ( );
    m_uRowCount = 1;
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( _Ty *ptr , size_t h , size_t w )
{
    if ( ptr )
    {
        m_uRowCount = h;
        m_uColumnCount = w;
        m_data.resize ( h*w );
        for ( size_t i = 0; i < h*w; i++ )
        {
            m_data [ i ] = ptr [ i ];
        }
    }
    else
    {
        throw runtime_error ( "empty pointer construction impossible\n" );
    }
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( vector<initializer_list<_Ty>> vals )
{
    vector<_Ty> row;
    size_t p = 0;
    for ( auto &i : vals )
    {
        p = ( p < i.size ( ) ) ? i.size ( ) : p;
    }
    m_uRowCount = vals.size ( );
    m_uColumnCount = p;
    for ( auto &i : vals )
    {
        row = i;
        for ( size_t x = 0; x < p; x++ )
        {
            if ( x < row.size ( ) )
            {
                m_data.push_back ( row [ x ] );
            }
            else
            {
                m_data.push_back ( 0 );
            }
        }
    }
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( vector<_Ty> &vec )
{
    m_data = vec;
    if ( ( m_data.size ( ) % 2 == 0 ) && m_data.size ( ) > 2 )
    {
        m_uColumnCount = m_data.size ( ) / 2;
        m_uRowCount = 2;
    }
    else
    {
        m_uColumnCount = m_data.size ( );
        m_uRowCount = 1;
    }
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( vector<_Ty> &vec , unsigned Row , unsigned Column )
{
    if ( ( Row * Column ) == vec.size ( ) )
    {
        m_uRowCount = Row;
        m_uColumnCount = Column;
        m_data = vec;
    }
    else
    {
        throw runtime_error ( "Object creation error : vec size mismatch!\n" );
    }
}
template<typename _Ty> CMatrix2D<_Ty>::CMatrix2D ( CMatrix2D<_Ty> &&src )
{
    this->m_data = src.m_data;
    this->m_uColumnCount = src.m_uColumnCount;
    this->m_uRowCount = src.m_uRowCount;
    src.m_data.clear ( );
    src.m_uColumnCount = 0;
    src.m_uRowCount = 0;
}
template<typename _Ty> void CMatrix2D<_Ty>::swap ( CMatrix2D<_Ty> &src )
{
    std::swap ( this->m_uColumnCount , src.m_uColumnCount );
    std::swap ( this->m_uRowCount , src.m_uRowCount );
    std::swap ( this->m_data , src.m_data );
}
template<typename _Ty> CMatrix2D<_Ty>& CMatrix2D<_Ty>::operator= ( const CMatrix2D<_Ty> &obj )
{
    if ( this != &obj )
    {
        CMatrix2D<_Ty> temp ( obj );
        this->swap ( temp );
    }
    return *this;
}
template<typename _Ty> CMatrix2D<_Ty>& CMatrix2D<_Ty>::operator=( CMatrix2D<_Ty> &&obj )
{
    if ( this != &obj )
    {
        this->m_data.clear ( );
        this->m_data = obj.m_data;
        this->m_uColumnCount = obj.m_uColumnCount;
        this->m_uRowCount = obj.m_uRowCount;
        obj.m_data.clear ( );
        obj.m_uColumnCount = 0;
        obj.m_uRowCount = 0;
    }
    return *this;
}
template<typename _Ty> void CMatrix2D<_Ty>::clear ( )
{
    m_uColumnCount = 0;
    m_uRowCount = 0;
    m_data.clear ( );
}
template<typename _Ty> CMatrix2D<_Ty>::~CMatrix2D ( )
{
    this->clear ( );
}
template<typename _Ty> void CMatrix2D<_Ty>::fill ( _Ty value )
{
    for ( auto &i : m_data )
    {
        i = value;
    }
}
template<typename _Ty> ostream &operator <<( ostream &os , const CMatrix2D<_Ty> &obj )
{
    for ( unsigned i = 0; i < obj.m_uRowCount; i++ )
    {
        for ( unsigned j = 0; j < obj.m_uColumnCount; j++ )
        {
            os << obj.m_data [ i*obj.m_uColumnCount + j ] << " ";
        }
        os << endl;
    }
    os << endl;
    return os;
}
template<typename _Ty> vector<_Ty> &CMatrix2D<_Ty>::data ( )
{
    return m_data;
}
template<typename _Ty> void CMatrix2D<_Ty>::resize ( unsigned uiNewrow , unsigned uiNewcolumn )
{
    if ( uiNewcolumn > 0 && uiNewrow > 0 )
    {
        if ( this->m_uColumnCount == 0 && this->m_uRowCount == 0 )
        {
            m_data.resize ( uiNewcolumn*uiNewrow );
        }
        else
        {
            vector<_Ty> tmp;
            unsigned pos = 0;
            if ( uiNewrow <= this->m_uRowCount && uiNewcolumn <= this->m_uColumnCount )
            {
                for ( unsigned i = 0; i < m_uRowCount; i++ )
                {
                    for ( unsigned j = 0; j < uiNewcolumn; j++ )
                    {
                        tmp.push_back ( this->m_data [ pos++ ] );
                    }
                    pos += m_uColumnCount - uiNewcolumn;
                }
                typename vector<_Ty>::iterator it = tmp.begin ( ) + uiNewrow*uiNewcolumn;
                tmp.erase ( it , tmp.end ( ) );
            }
            if ( uiNewrow > this->m_uRowCount && uiNewcolumn > this->m_uColumnCount )
            {
                for ( unsigned i = 0; i < m_uRowCount; i++ )
                {
                    for ( unsigned j = 0; j < uiNewcolumn; j++ )
                    {
                        ( j < m_uColumnCount ) ? tmp.push_back ( this->m_data [ pos++ ] ) : tmp.push_back ( _Ty ( ) );
                    }
                }
                for ( unsigned i = m_uRowCount*uiNewcolumn; i < uiNewcolumn*uiNewrow; i++ )
                {
                    tmp.push_back ( _Ty ( ) );
                }
            }
            if ( uiNewrow > this->m_uRowCount && uiNewcolumn <= this->m_uColumnCount )
            {
                for ( unsigned i = 0; i < m_uRowCount; i++ )
                {
                    for ( unsigned j = 0; j < uiNewcolumn; j++ )
                    {
                        tmp.push_back ( this->m_data [ pos++ ] );
                    }
                    pos += m_uColumnCount - uiNewcolumn;
                }
                for ( unsigned i = m_uRowCount*uiNewcolumn; i < uiNewcolumn*uiNewrow; i++ )
                {
                    tmp.push_back ( _Ty ( ) );
                }
            }
            if ( uiNewrow <= this->m_uRowCount && uiNewcolumn > this->m_uColumnCount )
            {
                for ( unsigned i = 0; i < m_uRowCount; i++ )
                {
                    for ( unsigned j = 0; j < uiNewcolumn; j++ )
                    {
                        ( j < m_uColumnCount ) ? tmp.push_back ( this->m_data [ pos++ ] ) : tmp.push_back ( _Ty ( ) );
                    }
                }
            }
            m_data = move ( tmp );
            //this->m_data = tmp;
        }
        this->m_uColumnCount = uiNewcolumn;
        this->m_uRowCount = uiNewrow;
    }
    else
    {
        throw runtime_error ( "Warning ! Attemp to resize to zero size! Resizing failed" );
    }
}

//template<typename _T1 , typename _T2>  bool CMatrix2D<_T1>::operator == ( const CMatrix2D<_T2>& other ) const
//{
//    if ( m_data.size ( ) != other.m_data.size ( ) || ( typeid(  ) != typeid( _T2 ) ) )
//    {
//        return false;
//    }
//    else
//    {
//        for ( size_t i = 0; i < m_data.size ( ); i++ )
//        {
//            if ( m_data [ i ] != other.m_data [ i ] )
//            {
//                return false;
//            }
//        }
//        return true;
//    }
//}
//template<typename _T1,typename _T2> bool CMatrix2D<_T1>::operator!=( const CMatrix2D<_T2>& other ) const
//{
//    return ( this == other ) ? false : true;
//}
template<typename _Ty> _Ty &CMatrix2D<_Ty>::operator()( unsigned y , unsigned x )
{
    assert ( ( y < m_uRowCount ) && ( x < m_uColumnCount ) );
    return m_data [ y*m_uColumnCount + x ];
}
template<typename _Ty> _Ty CMatrix2D<_Ty>::operator()( unsigned y , unsigned x ) const
{
    assert ( ( y < m_uRowCount ) && ( x < m_uColumnCount ) );
    return m_data [ y*m_uColumnCount + x ];
}
template<typename _Ty> void CMatrix2D<_Ty>::set ( unsigned row , unsigned column , _Ty value )
{
    if ( bSize ( row , column ) )
    {
        m_data [ row * m_uColumnCount + column ] = value;
    }
    else
    {
        throw runtime_error ( "Warning ! Out of range! Modification isn't possible." );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::fill_rnd ( )
{
    srand ( time ( NULL ) );
    for ( auto &i : m_data )
    {
        i = rand ( ) % ( m_data.size ( ) * 10 );
    }
}
template<typename _Ty> _Ty *CMatrix2D<_Ty>::get_max ( )
{
    if ( m_data.size ( ) )
    {
        _Ty *max = &m_data [ 0 ];
        for ( auto &i : m_data )
        {
            if ( i > *max )
            {
                max = &i;
            }
        }
        return max;
    }
    else
    {
        throw runtime_error ( "Fatal error! Empty Matrix" );
    }
}
template<typename _Ty> _Ty *CMatrix2D<_Ty>::get_min ( )
{
    if ( m_data.size ( ) )
    {
        _Ty *min = &m_data [ 0 ];
        for ( auto &i : m_data )
        {
            if ( i < *min )
            {
                min = &i;
            }
        }
        return min;
    }
    else
    {
        throw runtime_error ( "Fatal error! Empty Matrix" );
    }
}
template<typename _Ty> point<_Ty> CMatrix2D<_Ty>::get_max_pos ( )
{
    point<_Ty> tmp { 0 ,0 };
    if ( m_data.size ( ) )
    {
        _Ty *max = &m_data [ 0 ];
        for ( unsigned i = 0; i < m_uRowCount; i++ )
        {
            for ( unsigned j = 0; j < m_uColumnCount; j++ )
            {
                if ( m_data [ i*m_uColumnCount + j ] > *max )
                {
                    max = &m_data [ i*m_uColumnCount + j ];
                    tmp.m_First = i;
                    tmp.m_Second = j;
                }
            }
        }
        max = nullptr;
        delete max;
        return tmp;
    }
    else
    {
        throw runtime_error ( "Fatal Error ! Empty matrix!" );
    }
}
template<typename _Ty> point<_Ty> CMatrix2D<_Ty>::get_min_pos ( )
{
    point<_Ty> tmp { 0 ,0 };
    if ( m_data.size ( ) )
    {
        _Ty *min = &m_data [ 0 ];
        for ( unsigned i = 0; i < m_uRowCount; i++ )
        {
            for ( unsigned j = 0; j < m_uColumnCount; j++ )
            {
                if ( m_data [ i*m_uColumnCount + j ] < *min )
                {
                    min = &m_data [ i*m_uColumnCount + j ];
                    tmp.m_First = i;
                    tmp.m_Second = j;
                }
            }
        }
        min = nullptr;
        return tmp;
    }
    else
    {
        throw runtime_error ( "Fatal Error ! Empty matrix!" );
    }
}
template<typename _Ty> _Ty *CMatrix2D<_Ty>::at ( unsigned row , unsigned column )
{
    _Ty *tmp = nullptr;
    if ( bSize ( row , column ) )
    {
        tmp = &m_data [ row * m_uColumnCount + column ];
        return tmp;
    }
    else
    {
        throw runtime_error ( "Fatal Error ! Out of range! Modification isn't possible. args : " + to_string ( row ) + " " + to_string ( column ) );
    }
}
template<typename _Ty> unsigned CMatrix2D<_Ty>::size ( ) const
{
    return m_data.size ( );
}
template<typename _Ty> inline bool CMatrix2D<_Ty>::bSize ( unsigned row , unsigned column )
{
    if ( row < m_uRowCount && column < m_uColumnCount )
    {
        return true;
    }
    else
    {
        return false;
    }
}
template<typename _Ty> inline bool bSizeEqual ( const CMatrix2D<_Ty> &lhs , const CMatrix2D<_Ty> &rhs )
{
    if ( lhs.m_uColumnCount == rhs.m_uColumnCount && lhs.m_uRowCount == rhs.m_uRowCount )
    {
        return true;
    }
    else
    {
        return false;
    }
}
template<typename _Ty> vector<_Ty> CMatrix2D<_Ty>::get_row ( unsigned row )
{
    vector<_Ty> tmp;
    if ( bSize ( row , 1 ) )
    {
        for ( unsigned i = row*m_uColumnCount; i < ( row + 1 )*m_uColumnCount; i++ )
        {
            tmp.push_back ( m_data [ i ] );
        }
        return tmp;
    }
    else
    {
        throw runtime_error ( "Fatal Error ! Out of range!" );
    }
}
template<typename _Ty> vector<_Ty> CMatrix2D<_Ty>::get_column ( unsigned column )
{
    vector<_Ty> tmp;
    if ( bSize ( 1 , column ) )
    {
        for ( unsigned i = 0; i < m_uRowCount; i++ )
        {
            tmp.push_back ( m_data [ i*m_uColumnCount + column ] );
        }
        return tmp;
    }
    else
    {
        throw runtime_error ( "Fatal Error ! Out of range!" );
    }
}
template<typename _Ty> vector<_Ty> CMatrix2D<_Ty>::get_all ( ) const
{
    return m_data;
}
template<typename _Ty> CMatrix2D<_Ty> CMatrix2D<_Ty>::Hadamard ( const CMatrix2D<_Ty> &lhs , const CMatrix2D<_Ty> &rhs )
{
    if ( lhs.m_uColumnCount == rhs.m_uColumnCount && lhs.m_uRowCount == rhs.m_uRowCount )
    {
        CMatrix2D<_Ty> tmp ( lhs.m_uRowCount , lhs.m_uColumnCount );
        for ( unsigned i = 0; i < lhs.m_data.size ( ); i++ )
        {
            tmp.m_data [ i ] = lhs.m_data [ i ] * rhs.m_data [ i ];
        }
        return tmp;
    }
    else
    {
        throw runtime_error ( "Fatal Error ! Matrixes size mismatch!" );
    }
}
template<typename _Ty> CMatrix2D<_Ty> operator + ( const CMatrix2D<_Ty> &lhs , const CMatrix2D<_Ty> &rhs )
{
    if ( lhs.m_uColumnCount == rhs.m_uColumnCount && lhs.m_uRowCount == rhs.m_uRowCount )
    {
        CMatrix2D<_Ty> tmp ( lhs.m_uRowCount , lhs.m_uColumnCount );
        for ( unsigned i = 0; i < lhs.m_data.size ( ); i++ )
        {
            tmp.m_data [ i ] = lhs.m_data [ i ] + rhs.m_data [ i ];
        }
        return tmp;
    }
    else
    {
        throw runtime_error ( "Matrix::operator + : Fatal Error ! Matrixes size mismatch!" );
    }
}
template<typename _Ty> CMatrix2D<_Ty> operator + ( const CMatrix2D<_Ty> &obj , const _Ty value )
{
    CMatrix2D<_Ty> tmp ( obj.m_uRowCount , obj.m_uColumnCount );
    for ( unsigned i = 0; i < obj.m_data.size ( ); i++ )
    {
        tmp.m_data [ i ] = obj.m_data [ i ] + value;
    }
    return tmp;
}
template<typename _Ty> CMatrix2D<_Ty> operator - ( const CMatrix2D<_Ty> &lhs , const CMatrix2D<_Ty> &rhs )
{
    if ( lhs.m_uColumnCount == rhs.m_uColumnCount && lhs.m_uRowCount == rhs.m_uRowCount )
    {
        CMatrix2D<_Ty> tmp ( lhs.m_uRowCount , lhs.m_uColumnCount );
        for ( unsigned i = 0; i < lhs.m_data.size ( ); i++ )
        {
            tmp.m_data [ i ] = lhs.m_data [ i ] - rhs.m_data [ i ];
        }
        return tmp;
    }
    else
    {
        throw runtime_error ( "Matrix::operator - :Fatal Error ! Matrixes size mismatch!" );
    }
}
template<typename _Ty> CMatrix2D<_Ty> operator - ( const CMatrix2D<_Ty> &obj , const _Ty value )
{
    CMatrix2D<_Ty> tmp ( obj.m_uRowCount , obj.m_uColumnCount );
    for ( unsigned i = 0; i < obj.m_data.size ( ); i++ )
    {
        tmp.m_data [ i ] = obj.m_data [ i ] - value;
    }
    return tmp;
}
template<typename _Ty> CMatrix2D<_Ty> operator * ( const CMatrix2D<_Ty> &lhs , const CMatrix2D<_Ty> &rhs )
{
    if ( lhs.m_uColumnCount == rhs.m_uRowCount )
    {
        CMatrix2D<_Ty> tmp ( lhs.m_uRowCount , rhs.m_uColumnCount );
        for ( unsigned i = 0; i < lhs.m_uRowCount; i++ )
        {
            for ( unsigned j = 0; j < rhs.m_uColumnCount; j++ )
            {
                tmp.m_data [ i*rhs.m_uColumnCount + j ] = 0;
                for ( unsigned k = 0; k < rhs.m_uRowCount; k++ )
                {
                    tmp.m_data [ i*rhs.m_uColumnCount + j ] += lhs.m_data [ i*lhs.m_uColumnCount + k ] * rhs.m_data [ k * rhs.m_uColumnCount + j ];
                }
            }
        }
        return tmp;
    }
    else
    {
        throw runtime_error ( "Fatal Error ! Matrixes size mismatch!" );
    }
}
template<typename _Ty> CMatrix2D<_Ty> operator * ( const CMatrix2D<_Ty> &obj , _Ty value )
{
    CMatrix2D<_Ty> tmp ( obj.m_uRowCount , obj.m_uColumnCount );
    for ( unsigned i = 0; i < obj.m_data.size ( ); i++ )
    {
        tmp.m_data [ i ] = obj.m_data [ i ] * value;
    }
    return tmp;
}
template<typename _Ty> CMatrix2D<_Ty> operator / ( const CMatrix2D<_Ty> &obj , const _Ty value )
{
    CMatrix2D<_Ty> tmp ( obj.m_uRowCount , obj.m_uColumnCount );
    for ( unsigned i = 0; i < obj.m_data.size ( ); i++ )
    {
        tmp.m_data [ i ] = obj.m_data [ i ] / value;
    }
    return tmp;
}
template<typename _Ty> void CMatrix2D<_Ty>::insert_row ( const vector<_Ty> &vec , unsigned row )
{
    if ( vec.size ( ) == m_uColumnCount && row <= m_uRowCount )
    {
        for ( unsigned i = 0; i < vec.size ( ); i++ )
        {
            m_data.insert ( m_data.begin ( ) + i + row * m_uColumnCount , vec [ i ] );
        }
        this->m_uRowCount++;
    }
    else
    {
        throw runtime_error ( "Warning ! Insertion size mismatch ! Insertion failed!" );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::insert_column ( const vector<_Ty> &vec , unsigned column )
{
    if ( vec.size ( ) == m_uRowCount && column <= m_uColumnCount )
    {
        for ( unsigned i = 0; i < vec.size ( ); i++ )
        {
            m_data.insert ( m_data.begin ( ) + column + m_uColumnCount * i + i , vec [ i ] );
        }
        this->m_uColumnCount++;
    }
    else
    {
        throw runtime_error ( "Warning ! Insertion size mismatch ! Insertion failed!" );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::replace_column ( const vector<_Ty> &vec , unsigned column )
{
    if ( vec.size ( ) == m_uRowCount && column < m_uColumnCount )
    {
        for ( unsigned i = 0; i < vec.size ( ); i++ )
        {
            m_data [ column + m_uColumnCount * i ] = vec [ i ];
        }
    }
    else
    {
        throw runtime_error ( "Warning ! Replace size mismatch ! Replace failed!" );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::replace_row ( const vector<_Ty> &vec , unsigned row )
{
    if ( vec.size ( ) == m_uColumnCount && row < m_uRowCount )
    {
        for ( unsigned i = 0; i < vec.size ( ); i++ )
        {
            m_data [ row * m_uColumnCount + i ] = vec [ i ];
        }
    }
    else
    {
        throw runtime_error ( "Warning ! Replace size mismatch ! Replace failed!" );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::transpose ( )
{
    vector<_Ty> tmp ( m_uColumnCount*m_uRowCount , 0 );
    for ( unsigned i = 0; i < m_uRowCount; i++ )
    {
        for ( unsigned j = 0; j < m_uColumnCount; j++ )
        {
            std::swap ( m_data [ i*m_uColumnCount + j ] , tmp [ j*m_uRowCount + i ] );
        }
    }
    std::swap ( tmp , this->m_data );
    std::swap ( this->m_uRowCount , this->m_uColumnCount );
}
template<typename _Ty> bool CMatrix2D<_Ty>::isSquare ( ) const
{
    return ( m_uColumnCount == m_uRowCount ) ? true : false;
}
template<typename _Ty> void CMatrix2D<_Ty>::reverse_row ( unsigned row )
{
    if ( row < m_uRowCount )
    {
        std::reverse ( m_data.begin ( ) + m_uColumnCount * row , m_data.begin ( ) + m_uColumnCount * ( row + 1 ) );
    }
    else
    {
        throw runtime_error ( "Warning !Incorrect Row ID !Reverse failed!\n" );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::reverse_column ( unsigned column )
{
    if ( column < m_uColumnCount )
    {
        for ( unsigned i = 0; i < m_uRowCount / 2; i++ )
        {
            std::swap ( m_data [ column + i*m_uColumnCount ] , m_data [ column + ( m_uRowCount - i - 1 )*m_uColumnCount ] );
        }
    }
    else
    {
        throw runtime_error ( "Warning ! Incorrect Column ID ! Reverse failed!" );
    }
}
template<typename _Ty> _Ty CMatrix2D<_Ty>::abs ( _Ty value )
{
    return ( value < 0 ) ? value * ( -1 ) : value;
}
template<typename _Ty> void CMatrix2D<_Ty>::get_memory_size ( ) const
{
    cout << "Matrix size : " << m_uRowCount << " x " << m_uColumnCount << endl;
    cout << "Memory using : " << sizeof ( _Ty ) * m_data.size ( ) << " bytes\n";
}
template<typename _Ty> void CMatrix2D<_Ty>::reverse_column_all ( )
{
    for ( unsigned i = 0; i < m_uColumnCount; i++ )
    {
        this->reverse_column ( i );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::reverse_rows_all ( )
{
    for ( unsigned i = 0; i < m_uRowCount; i++ )
    {
        this->reverse_row ( i );
    }
}
template<typename _Ty> point<unsigned> CMatrix2D<_Ty>::get_size ( ) const
{
    return point < unsigned > {m_uRowCount , m_uColumnCount};
}
template<typename _Ty> void CMatrix2D<_Ty>::normalize ( )
{
    if ( m_data.size ( ) )
    {
        _Ty abs_max = this->abs ( m_data [ 0 ] );
        _Ty tmp;
        for ( auto &i : m_data )
        {
            tmp = this->abs ( i );
            if ( tmp > abs_max )
            {
                abs_max = tmp;
            }
        }
        for ( auto &i : m_data )
        {
            i /= abs_max;
        }
    }
    else
    {
        throw runtime_error ( "Warning ! Empty matrix! " );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::remove_column ( unsigned column )
{
    if ( column < m_uColumnCount )
    {
        for ( auto i = 0; i < m_uRowCount; i++ )
        {
            m_data.erase ( m_data.begin ( ) + i*m_uColumnCount + column - i );
        }
        m_uColumnCount--;
        this->get_memory_size ( );
    }
    else
    {
        throw runtime_error ( "Out of range!\n" );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::remove_row ( unsigned row )
{
    if ( row < m_uRowCount )
    {
        m_data.erase ( m_data.begin ( ) + row * m_uColumnCount , m_data.begin ( ) + ( row + 1 )*( m_uColumnCount ) );
        m_uRowCount--;
        this->get_memory_size ( );
    }
    else
    {
        throw runtime_error ( "Out of range!\n" );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::decimation ( unsigned ratio , bool type )
{
    CMatrix2D<_Ty> temp;
    int counter = 0;
    switch ( ratio )
    {
        case Double:
            temp.resize ( m_uRowCount / 2 , m_uColumnCount / 2 );
            for ( size_t i = 0; i < m_uRowCount; i += 2 )
            {
                for ( size_t j = 0; j < m_uColumnCount; j += 2 )
                {
                    if ( ( i + 1 < m_uRowCount ) && ( j + 1 < m_uColumnCount ) )
                    {
                        if ( type )
                        {
                            temp [ counter++ ] = ( *at ( i , j ) + *at ( i + 1 , j ) + *at ( i , j + 1 ) + *at ( i + 1 , j + 1 ) ) / 4;
                        }
                        else
                        {
                            temp [ counter++ ] = *at ( i , j );
                        }
                    }
                }
            }
            *this = std::move ( temp );
            break;
        case Quad:
            temp.resize ( m_uRowCount / 4 , m_uColumnCount / 4 );
            for ( size_t i = 0; i < m_uRowCount; i += 4 )
            {
                for ( size_t j = 0; j < m_uColumnCount; j += 4 )
                {
                    if ( ( i + 1 < m_uRowCount ) && ( j + 1 < m_uColumnCount ) )
                    {
                        if ( ( i + 3 < m_uRowCount ) && ( j + 3 < m_uColumnCount ) )
                        {
                            if ( type )
                            {
                                temp [ counter++ ] = (
                                    *at ( i , j ) + *at ( i , j + 1 ) + *at ( i , j + 2 ) + *at ( i , j + 3 ) +
                                    *at ( i + 1 , j ) + *at ( i + 1 , j + 1 ) + *at ( i + 1 , j + 2 ) + *at ( i + 1 , j + 3 ) +
                                    *at ( i + 2 , j ) + *at ( i + 2 , j + 1 ) + *at ( i + 2 , j + 2 ) + *at ( i + 2 , j + 3 ) +
                                    *at ( i + 3 , j ) + *at ( i + 3 , j + 1 ) + *at ( i + 3 , j + 2 ) + *at ( i + 3 , j + 3 ) ) / 16;
                            }
                            else
                            {
                                temp [ counter++ ] = *at ( i , j );
                            }
                        }
                    }
                }
            }
            *this = std::move ( temp );
            break;
        default:
            throw runtime_error ( "decimation wrong ratio\n" );
            break;
    }
}
template<typename _Ty> _Ty CMatrix2D<_Ty>::sum ( CMatrix2D<_Ty> &obj )
{
    _Ty sum ( 0 );
    for ( auto i : obj )
    {
        sum += i;
    }
    return sum;
}
template<typename _Ty> void CMatrix2D<_Ty>::upscaling ( unsigned ratio )
{
    assert ( ratio > 0 );
    CMatrix2D<_Ty> temp;
    int counter = 0;
    temp.resize ( m_uRowCount * ratio , m_uColumnCount * ratio );
    for ( size_t i = 0; i < m_uRowCount * ratio; i += ratio )
    {
        for ( size_t j = 0; j < m_uColumnCount * ratio; j += ratio )
        {
            for ( size_t k = 0; k < ratio; k++ )
            {
                for ( size_t l = 0; l < ratio; l++ )
                {
                    temp.set ( i + k , j + l , m_data [ counter ] );
                }
            }
            counter++;
        }
    }
    *this = std::move ( temp );
}
template<typename _Ty> void CMatrix2D<_Ty>::y_axis_shift ( int y )
{
    if ( abs ( y ) < m_uRowCount )
    {
        CMatrix2D<_Ty> t_matrix ( m_uRowCount - abs ( y ) , m_uColumnCount );
        for ( auto i = 0; i < m_uRowCount; i++ )
        {
            for ( auto j = 0; j < m_uColumnCount; j++ )
            {
                if ( y >= 0 )
                {
                    if ( t_matrix.get_size ( ).m_First > i )
                    {
                        t_matrix.set ( i , j , *at ( i , j ) );
                    }
                }
                else
                {
                    if ( i >= abs ( y ) )
                    {
                        t_matrix.set ( i + y , j , *at ( i , j ) );
                    }
                }
            }
        }
        *this = move ( t_matrix );
    }
    else
    {
        *this = CMatrix2D < _Ty > ( );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::x_axis_shift ( int x )
{
    if ( abs ( x ) < m_uColumnCount )
    {
        CMatrix2D<_Ty> t_matrix ( m_uRowCount , m_uColumnCount - abs ( x ) );
        for ( auto i = 0; i < m_uRowCount; i++ )
        {
            for ( auto j = 0; j < m_uColumnCount; j++ )
            {
                if ( x >= 0 )
                {
                    if ( t_matrix.get_size ( ).m_Second > j )
                    {
                        t_matrix.set ( i , j , *at ( i , j ) );
                    }
                }
                else
                {
                    if ( j >= abs ( x ) )
                    {
                        t_matrix.set ( i , j + x , *at ( i , j ) );
                    }
                }
            }
        }
        *this = move ( t_matrix );
    }
    else
    {
        *this = CMatrix2D<_Ty> ( );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::DPCM ( unsigned Mode )
{
    assert ( Mode < Average_Neib + 1 );
    CMatrix2D<_Ty> temp ( *this );
    for ( size_t i = 1; i < m_uRowCount; i++ )
    {
        for ( size_t j = 1; j < m_uColumnCount; j++ )
        {
            switch ( Mode )
            {
                case Left:
                    *temp.at ( i , j ) = *at ( i , j ) - *at ( i , j - 1 );
                    break;
                case Top:
                    *temp.at ( i , j ) = *at ( i , j ) - *at ( i - 1 , j );
                    break;
                case TopLeft:
                    *temp.at ( i , j ) = *at ( i , j ) - *at ( i - 1 , j - 1 );
                    break;
                case Average_Neib:
                    *temp.at ( i , j ) = *at ( i , j ) - ( ( *at ( i - 1 , j ) + *at ( i - 1 , j - 1 ) + *at ( i , j - 1 ) ) / 3 );
                    break;
                default:
                    break;
            }
        }
    }
    *this = move ( temp );
}
template<typename _Ty> CMatrix2D<_Ty> CMatrix2D<_Ty>::GetSquare ( unsigned y , unsigned x , unsigned radius )
{
    CMatrix2D<_Ty> tmp ( 2 * radius + 1 , 2 * radius + 1 );
    int x_s = 0 , y_s = 0;
    for ( auto i = y - radius; i < y - radius + tmp.GetHeight ( ); i++ )
    {
        for ( auto j = x - radius; j < x - radius + tmp.GetWidth ( ); j++ )
        {
            tmp ( y_s , x_s++ ) = *at ( i , j );
        }
        y_s++;
        x_s = 0;
    }
    return tmp;
}
template<typename _Ty> CMatrix2D<_Ty*> CMatrix2D<_Ty>::sqr ( size_t y , size_t x , size_t tall , size_t wide )
{
    CMatrix2D<_Ty*> temp ( tall , wide );
    if ( y + tall < m_uRowCount  && x + wide < m_uColumnCount )
    {
        size_t h = 0 , w = 0;
        for ( size_t i = y; i < y + tall; i++ )
        {
            for ( size_t j = x; j < x + wide; j++ )
            {
                temp ( h , w++ ) = at ( i , j );
            }
            w = 0;
            h++;
        }
    }
    return temp;
}
template<typename _Ty> CMatrix2D<_Ty> CMatrix2D<_Ty>::rect ( size_t y , size_t x , size_t tall , size_t wide )
{
    CMatrix2D<_Ty> temp ( tall , wide );
    if ( y + tall <= m_uRowCount  && x + wide <= m_uColumnCount )
    {
        size_t h = 0 , w = 0;
        for ( size_t i = y; i < y + tall; i++ )
        {
            w = 0;
            for ( size_t j = x; j < x + wide; j++ )
            {
                temp ( h , w++ ) = *at ( i , j );
            }
            h++;
        }
    }
    else
    {
        throw runtime_error ( to_string ( y ) + " " + to_string ( x ) + " " + to_string ( tall ) + " " + to_string ( wide ) + "unable to create rect\n" );
    }
    return temp;
}
template<typename _Ty> void CMatrix2D<_Ty>::right_append ( CMatrix2D<_Ty>& mtrx , _Ty _value )
{
    if ( this->m_uRowCount == mtrx.GetHeight ( ) )
    {
        resize ( m_uRowCount , m_uColumnCount + 1 );
        for ( size_t i = 0; i < m_uRowCount; i++ )
        {
            *at ( i , m_uColumnCount - 1 ) = _value;
        }
        resize ( m_uRowCount , m_uColumnCount + mtrx.GetWidth ( ) );
        size_t W = 0;
        for ( size_t i = 0; i < m_uRowCount; i++ )
        {
            W = 0;
            for ( size_t j = m_uColumnCount - mtrx.GetWidth ( ); j < m_uColumnCount; j++ )
            {
                *at ( i , j ) = mtrx ( i , W++ );
            }
        }
    }
    else
    {
        throw runtime_error ( "Matrix height mismatch\n" );
    }
}

template<typename _Ty> void CMatrix2D<_Ty>::emplace ( CMatrix2D<_Ty> &mtrx , size_t y , size_t x )
{
    if ( mtrx.GetHeight ( ) + y < GetHeight ( ) && mtrx.GetWidth ( ) + x < GetWidth ( ) )
    {
        size_t line = 0;
        for ( size_t i = y; i < y + mtrx.GetHeight ( ); i++ )
        {
            size_t colunm = 0;
            for ( size_t j = x; j < x + mtrx.GetWidth ( ); j++ )
            {
                *at ( i , j ) = mtrx ( line , colunm++ );
            }
            line++;
        }

    }
    else
    {
        runtime_error ( "fall in matrix::emplace function\n" );
    }
}
template<typename _Ty> void CMatrix2D<_Ty>::decimate (unsigned ratio, unsigned start_pos )
{
    if ( ( ratio < GetHeight ( ) && ratio < GetWidth ( ) ) && ( start_pos < GetHeight ( ) && start_pos < GetWidth ( ) ) )
    {
        size_t tall = 0 , width = 0;
        for ( size_t i = start_pos; i < GetHeight ( ); i += ratio , tall++ );
        for ( size_t i = start_pos; i < GetWidth ( ); i += ratio , width++ );
        CMatrix2D<_Ty> mtrx ( tall , width );
        size_t y_pos = 0;
        for ( size_t i = start_pos; i < GetHeight ( ); i += ratio )
        {
            size_t x_pos = 0;
            for ( size_t j = start_pos; j < GetWidth ( ); j += ratio )
            {
                mtrx ( y_pos , x_pos++ ) = *at( i , j );
            }
            y_pos++;
        }
        *this = move ( mtrx );
    }
    else
    {
        throw runtime_error ( "CMatrix2D<>::decimate - out of range\n" );
    }

}
#endif
