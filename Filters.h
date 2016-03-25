#include "BMP.h"
#include <algorithm>
#define Filter CFilter *
abstract_class CFilter
{
public:
    CFilter ( );
    virtual ~CFilter ( );
    virtual void Apply ( CBMP & ) = 0;
    virtual void Apply ( CMatrix2D<int> & ) = 0;
    CMatrix2D<float> GetCore ( );
    virtual void SetKernel ( CMatrix2D<float> & )
    {

    }
    void SetRadius ( unsigned );
    unsigned GetRadius ( );
protected:
    CMatrix2D<float> m_Kernel;
    float m_Multiplier;
    unsigned m_Radius;
};
class AdaptiveFilter : public CFilter
{
public:
    AdaptiveFilter ( )
    {

    }
    virtual ~AdaptiveFilter ( )
    {

    }
    virtual void SetKernel ( CMatrix2D<float> &obj )
    {
        if ( obj.GetHeight ( ) == obj.GetWidth ( ) && ( obj.GetHeight ( ) % 2 ) )
        {
            m_Kernel = obj;
            m_Radius = ( obj.GetHeight ( ) - 1 ) / 2;
        }
        else
        {
            throw runtime_error ( "incorrect filter setup\n" );
        }
    }
    virtual void Apply ( CMatrix2D<int> & )
    {
        //stub
    }
    virtual void Apply ( CBMP &obj )
    {
        const int OFFSET = 2;
        double summary;
        int primary_height = obj.GetHeight ( );
        int primary_width = obj.GetWidth ( );
        CMatrix2D<rgbD> temp_image = fromVector (
            bicubicresize (
            fromImage ( dynamic_cast< BMP24& >( obj ) ) ,
            obj.GetWidth ( ) , obj.GetHeight ( ) , obj.GetWidth ( ) + 2 * OFFSET , obj.GetHeight ( ) + 2 * OFFSET )
            , std::move(obj.GetHeight ( ) + 2 * OFFSET) , std::move(obj.GetWidth ( ) + 2 * OFFSET) );
        CMatrix2D<rgbD> original = obj.GetPixelData ( );
        for ( auto i = 0; i < original.GetHeight ( ); i++ )
        {
            for ( auto j = 0; j < original.GetWidth ( ); j++ )
            {
                *temp_image.at ( i + OFFSET , j + OFFSET ) = original ( i , j );
            }
        }
        obj.SetPixelData ( temp_image );
        int height_bounds = obj.GetHeight ( );
        int width_bounds = obj.GetWidth ( );
        int core_height_bounds = m_Kernel.GetHeight ( );
        int core_width_bounds = m_Kernel.GetWidth ( );
        int half_height_core = m_Radius + 1;
        int half_width_core = m_Radius + 1;
        assert ( height_bounds > 0 && width_bounds > 0 );
        CMatrix2D<int> f [ 3 ];
        for ( auto i = 0; i < 3; i++ )
        {
            CMatrix2D<int> source ( obj.GetComponentPixmap ( i ) );
            CMatrix2D<int> dest ( obj.GetComponentPixmap ( i ) );
            f [ i ] = CMatrix2D<int> ( primary_height , primary_width );
            for ( auto j = half_height_core; j < height_bounds - half_height_core; j++ )
            {
                for ( auto k = half_width_core; k < width_bounds - half_width_core; k++ )
                {
                    summary = 0;
                    for ( auto y = 0; y < core_height_bounds; y++ )
                    {
                        for ( auto x = 0; x < core_width_bounds; x++ )
                        {
                            summary += source ( j + y - half_height_core , k + x - half_width_core )*m_Kernel ( y , x );
                        }
                    }
                    *f [ i ].at ( j - half_height_core , k - half_height_core ) = summary;
                    check_bounds ( *f [ i ].at ( j - half_height_core , k - half_height_core ) );
                    //dest ( j ,k ) = summary * m_Multiplier;
                }
            }
            /*       for ( auto j = OFFSET; j < dest.GetHeight ( ) - OFFSET; j++ )
            {
            for ( auto k = OFFSET; k < dest.GetWidth ( ) - OFFSET; k++ )
            {
            *f[i].at ( j - OFFSET, k - OFFSET ) = *dest.at ( j, k );

            }
            }*/
            //obj.SetComponentData ( dest ,i );
        }
        CMatrix2D<rgbD> output ( f [ 0 ].GetHeight ( ) , f [ 0 ].GetWidth ( ) );
        for ( auto i = 0; i < output.GetHeight ( ); i++ )
        {
            for ( auto j = 0; j < output.GetWidth ( ); j++ )
            {
                output.at ( i , j )->Red = *f [ 0 ].at ( i , j );
                output.at ( i , j )->Green = *f [ 1 ].at ( i , j );
                output.at ( i , j )->Blue = *f [ 2 ].at ( i , j );
            }
        }
        obj.SetPixelData ( output );
    }
    virtual void Apply ( CMatrix2D<rgbD> &entry )
    {
        BMP24 out;
        out.SetPixelData ( entry );
        this->Apply ( out );
        entry = out.GetPixelData ( );
    }

};
class Gaussian : public CFilter
{
public:
    Gaussian ( );
    Gaussian ( double );
    ~Gaussian ( );
    virtual void Apply ( CBMP & );
    virtual void Apply ( CMatrix2D<int> & );
};
class Average : public CFilter
{
public:
    Average ( );
    Average ( int );
    ~Average ( );
    virtual void Apply ( CMatrix2D<int> & );
    virtual void Apply ( CBMP & );
};
class Median : public CFilter
{
public:
    Median ( );
    Median ( int );
    ~Median ( );
    virtual void Apply ( CBMP & );
    virtual void Apply ( CMatrix2D<int> & );
};
class SobelOperator : public CFilter
{
public:
    SobelOperator ( );
    ~SobelOperator ( );
    virtual void Apply ( CBMP & );
    virtual void Apply ( CMatrix2D<int> & );
private:
    CMatrix2D<float> m_KernelY;
};
class LaplaceOperator : public CFilter
{
public:
    LaplaceOperator ( bool );
    ~LaplaceOperator ( );
    virtual void Apply ( CBMP & );
    virtual void Apply ( CMatrix2D<int> & );
};
