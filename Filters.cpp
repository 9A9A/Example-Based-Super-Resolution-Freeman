#include "Filters.h"
#include <omp.h>
CFilter::CFilter ( )
{
    m_Multiplier = 0;
    m_Radius = 0;
}
CFilter::~CFilter ( )
{
    m_Kernel.clear ( );
    m_Multiplier = 0;
    m_Radius = 0;
}
Gaussian::Gaussian ( )
{
    m_Kernel.resize ( 3 , 3 );
    m_Kernel ( 0 , 0 ) = 1;
    m_Kernel ( 0 , 1 ) = 2;
    m_Kernel ( 0 , 2 ) = 1;
    m_Kernel ( 1 , 0 ) = 2;
    m_Kernel ( 1 , 1 ) = 4;
    m_Kernel ( 1 , 2 ) = 2;
    m_Kernel ( 2 , 0 ) = 1;
    m_Kernel ( 2 , 1 ) = 2;
    m_Kernel ( 2 , 2 ) = 1;
    m_Multiplier = 1.0f / 16.0f;
    m_Radius = 1;
}
Gaussian::Gaussian ( double Sigma )
{
    m_Radius = ceil ( 3 * Sigma );
    m_Kernel.resize ( 2 * m_Radius + 1 , 2 * m_Radius + 1 );
    int height = m_Kernel.GetHeight ( );
    int width = m_Kernel.GetWidth ( );
    double sum = 0;
    auto gaussian = [ Sigma ] ( double x , double mu ) -> double
    {
        return exp ( -( ( ( x - mu ) / ( Sigma ) )*( ( x - mu ) / ( Sigma ) ) ) / 2.0 );
    };
    for ( auto i = 0; i < height; i++ )
    {
        for ( auto j = 0; j < width; j++ )
        {
            m_Kernel ( i , j ) = gaussian ( i , m_Radius )*gaussian ( j , m_Radius );
            sum += m_Kernel ( i , j );
        }
    }
    for ( auto &i : m_Kernel )
    {
        i /= sum;
    }
    m_Multiplier = 1;
}
Gaussian::~Gaussian ( )
{
}
void Gaussian::Apply ( CBMP &obj )
{
    //SINGLE THREADED
    const int OFFSET = 2;
    double summary;
    int primary_height = obj.GetHeight ( );
    int primary_width = obj.GetWidth ( );
    CMatrix2D<rgbD> temp_image = fromVector (
        bicubicresize (
        fromImage ( dynamic_cast< BMP24& >( obj ) ) ,
        obj.GetWidth ( ) , obj.GetHeight ( ) , obj.GetWidth ( ) + 2 * OFFSET , obj.GetHeight ( ) + 2 * OFFSET )
        , obj.GetHeight ( ) + 2 * OFFSET , obj.GetWidth ( ) + 2 * OFFSET );
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
                *f [ i ].at ( j - half_height_core , k - half_height_core ) = summary*m_Multiplier;
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
    obj.AddToFilename ( string ( "_gaussian" ) );
}
void Gaussian::Apply ( CMatrix2D<int> &obj )
{
    //SINGLE THREADED
    int height_bounds = obj.GetHeight ( );
    int width_bounds = obj.GetWidth ( );
    int core_height_bounds = m_Kernel.GetHeight ( );
    int core_width_bounds = m_Kernel.GetWidth ( );
    int half_height_core = m_Radius + 1;
    int half_width_core = m_Radius + 1;
    assert ( height_bounds > 0 && width_bounds > 0 );
    int summary;
    CMatrix2D<int> dest ( height_bounds , width_bounds , 0 );
    for ( auto j = half_height_core; j < height_bounds - half_height_core; j++ )
    {
        for ( auto k = half_width_core; k < width_bounds - half_width_core; k++ )
        {
            summary = 0;
            for ( auto y = 0; y < core_height_bounds; y++ )
            {
                for ( auto x = 0; x < core_width_bounds; x++ )
                {
                    summary += obj ( j + y - half_height_core , k + x - half_width_core ) * m_Kernel ( y , x );
                }
            }
            dest ( j , k ) = summary * m_Multiplier;
        }
    }
    obj = std::move ( dest );
}
Median::Median ( )
{
}
Median::Median ( int Radius )
{
    m_Radius = Radius;
    m_Kernel.resize ( 2 * m_Radius + 1 , 2 * m_Radius + 1 );
}
Median::~Median ( )
{
}
void Median::Apply ( CMatrix2D<int> &obj )
{
    double start = omp_get_wtime ( );
    int height_bounds = obj.GetHeight ( );
    int width_bounds = obj.GetWidth ( );
    int core_height_bounds = m_Kernel.GetHeight ( );
    int core_width_bounds = m_Kernel.GetWidth ( );
    int half_height_core = m_Radius + 1;
    int half_width_core = m_Radius + 1;
    int pos = ( pow ( 2 * m_Radius + 1 , 2 ) / 2 ) + 1;
    assert ( height_bounds > 0 && width_bounds > 0 );
    CMatrix2D<int> dest ( obj );
    //for ( auto j = half_height_core; j < height_bounds - half_height_core; j++ )
    //{
    //	for ( auto k = half_width_core; k < width_bounds - half_width_core; k++ )
    //	{
    //		vector<int> temp;
    //		for ( auto y = 0; y < core_height_bounds; y++ )
    //		{
    //			for ( auto x = 0; x < core_width_bounds; x++ )
    //			{
    //				temp.push_back ( obj ( j + y - half_height_core ,k + x - half_width_core ) );
    //			}
    //		}
    //		sort ( temp.begin ( ) ,temp.end ( ) );
    //		dest ( j ,k ) = temp[ ( pow ( 2 * m_Radius + 1 ,2 ) / 2 ) + 1 ];
    //	}
    //}
    double coeff = ( 1.0f / pow ( core_height_bounds , 2 ) );
    auto sub_thread =
        [ coeff , height_bounds , width_bounds , core_height_bounds , core_width_bounds , half_height_core , half_width_core , &obj , &dest , pos ]
    ( int s_y , int s_x , int e_y , int e_x )-> void
    {
        int summary = 0;
        for ( auto j = s_y; j < e_y; j++ )
        {
            for ( auto k = s_x; k < e_x; k++ )
            {
                vector<int> temp;
                for ( auto y = 0; y < core_height_bounds; y++ )
                {
                    for ( auto x = 0; x < core_width_bounds; x++ )
                    {
                        temp.push_back ( obj ( j + y - half_height_core , k + x - half_width_core ) );
                    }
                }
                sort ( temp.begin ( ) , temp.end ( ) );
                dest ( j , k ) = temp [ pos ];
            }
        }
    };
    thread st_thread ( sub_thread ,
                       half_height_core ,
                       half_width_core ,
                       ( height_bounds - half_height_core ) / 2 ,
                       ( width_bounds - half_width_core ) / 2 );
    thread nd_thread ( sub_thread ,
                       ( height_bounds - half_height_core ) / 2 ,
                       half_height_core ,
                       height_bounds - half_height_core ,
                       ( width_bounds - half_width_core ) / 2 );
    thread rd_thread ( sub_thread ,
                       half_height_core ,
                       ( width_bounds - half_width_core ) / 2 ,
                       ( height_bounds - half_height_core ) / 2 ,
                       width_bounds - half_width_core );
    thread th_thread ( sub_thread ,
                       ( height_bounds - half_height_core ) / 2 ,
                       ( width_bounds - half_width_core ) / 2 ,
                       height_bounds - half_height_core ,
                       width_bounds - half_width_core );
    st_thread.join ( );
    nd_thread.join ( );
    rd_thread.join ( );
    th_thread.join ( );
    obj = std::move ( dest );
    double end = omp_get_wtime ( );
#ifdef _DEBUG
    cout << "Median filter ( Radius : " << m_Radius << " ) Seconds : " << end - start << endl;
#endif
}
void Median::Apply ( CBMP &obj )
{
    int height_bounds = obj.GetHeight ( );
    int width_bounds = obj.GetWidth ( );
    int core_height_bounds = m_Kernel.GetHeight ( );
    int core_width_bounds = m_Kernel.GetWidth ( );
    int half_height_core = m_Radius + 1;
    int half_width_core = m_Radius + 1;
    assert ( height_bounds > 0 && width_bounds > 0 );
    for ( auto i = 0; i < 3; i++ )
    {
        CMatrix2D<int> source ( obj.GetComponentPixmap ( i ) );
        CMatrix2D<int> dest ( obj.GetComponentPixmap ( i ) );
        for ( auto j = half_height_core; j < height_bounds - half_height_core; j++ )
        {
            for ( auto k = half_width_core; k < width_bounds - half_width_core; k++ )
            {
                vector<int> temp;
                for ( auto y = 0; y < core_height_bounds; y++ )
                {
                    for ( auto x = 0; x < core_width_bounds; x++ )
                    {
                        temp.push_back ( source ( j + y - half_height_core , k + x - half_width_core ) );
                    }
                }
                sort ( temp.begin ( ) , temp.end ( ) );
                dest ( j , k ) = temp [ ( pow ( 2 * m_Radius + 1 , 2 ) / 2 ) + 1 ];
            }
        }
        obj.SetComponentData ( dest , i );
    }
    obj.AddToFilename ( string ( "_powerstat" ) );
}
Average::Average ( )
{
}
Average::~Average ( )
{
}
Average::Average ( int Radius )
{
    m_Radius = Radius;
}
void Average::Apply ( CMatrix2D<int> &obj )
{
    int height_bounds = obj.GetHeight ( );
    int width_bounds = obj.GetWidth ( );
    int core_height_bounds = 2 * m_Radius + 1;
    int core_width_bounds = 2 * m_Radius + 1;
    int half_height_core = m_Radius + 1;
    int half_width_core = m_Radius + 1;
    assert ( height_bounds > 0 && width_bounds > 0 );
    int summary;
    CMatrix2D<int> dest ( obj );
    for ( auto j = half_height_core; j < height_bounds - half_height_core; j++ )
    {
        for ( auto k = half_width_core; k < width_bounds - half_width_core; k++ )
        {
            summary = 0;
            for ( auto y = 0; y < core_height_bounds; y++ )
            {
                for ( auto x = 0; x < core_width_bounds; x++ )
                {
                    summary += obj ( j + y - half_height_core , k + x - half_width_core );
                }
            }
            dest ( j , k ) = summary * ( 1.0f / pow ( 2 * m_Radius + 1 , 2 ) );
            check_bounds ( dest ( j , k ) );
        }
    }
    obj = std::move ( dest );
}
void Average::Apply ( CBMP &obj )
{
    double start = omp_get_wtime ( );
    //int height_bounds = obj.GetHeight ( );
    //int width_bounds = obj.GetWidth ( );
    //int core_height_bounds = 2*m_Radius+1;
    //int core_width_bounds = 2*m_Radius+1;
    //int half_height_core = m_Radius + 1;
    //int half_width_core = m_Radius + 1;
    //assert ( height_bounds > 0 && width_bounds > 0 );
    //int summary;
    //for ( auto i = 0; i < 3; i++ )
    //{
    //	CMatrix2D<int> source ( obj.GetComponentPixmap ( i ) );
    //	CMatrix2D<int> dest ( obj.GetComponentPixmap ( i ) );
    //	for ( auto j = half_height_core; j < height_bounds - half_height_core; j++ )
    //	{
    //		for ( auto k = half_width_core; k < width_bounds - half_width_core; k++ )
    //		{
    //			summary = 0;
    //			for ( auto y = 0; y < core_height_bounds; y++ )
    //			{
    //				for ( auto x = 0; x < core_width_bounds; x++ )
    //				{
    //					summary += source ( j + y - half_height_core ,k + x - half_width_core );
    //				}
    //			}
    //			dest ( j ,k ) = summary * ( 1.0f / pow(2 * m_Radius + 1,2 ));
    //			check_bounds ( dest ( j ,k ) );
    //		}
    //	}
    //	obj.SetComponentData ( dest ,i );
    //}
    //obj.AddToFilename ( string ( "_average" ) ) ;
    //int height_bounds = obj.GetHeight ( );
    //int width_bounds = obj.GetWidth ( );
    //int core_height_bounds = 2 * m_Radius + 1;
    //int core_width_bounds = 2 * m_Radius + 1;
    //int half_height_core = m_Radius + 1;
    //int half_width_core = m_Radius + 1;
    //double coeff = ( 1.0f / pow ( core_height_bounds ,2 ) );
    //assert ( height_bounds > 0 && width_bounds > 0 );
    //int summary;
    //auto single_thread = 
    //	[ coeff,height_bounds ,width_bounds ,core_height_bounds ,core_width_bounds ,half_height_core ,half_width_core, &obj ] 
    //	( int component ) -> void
    //{
    //	CMatrix2D<int> source ( obj.GetComponentPixmap ( component ) );
    //	CMatrix2D<int> dest ( source );
    //	auto sub_thread = 
    //		[ coeff,height_bounds ,width_bounds ,core_height_bounds ,core_width_bounds ,half_height_core ,half_width_core ,&obj ,&source ,&dest ] 
    //		( int s_y ,int s_x ,int e_y ,int e_x )-> void
    //	{
    //		int summary = 0;
    //		for ( auto j = s_y; j < e_y; j++ )
    //		{
    //			for ( auto k = s_x; k <  e_x; k++ )
    //			{
    //				summary = 0;
    //				for ( auto y = 0; y < core_height_bounds; y++ )
    //				{
    //					for ( auto x = 0; x < core_width_bounds; x++ )
    //					{
    //						summary += source ( j + y - half_height_core ,k + x - half_width_core );
    //					}
    //				}
    //				dest ( j ,k ) = summary * coeff;
    //				check_bounds ( dest ( j ,k ) );
    //			}
    //		}
    //	};
    //	thread st_thread ( sub_thread ,
    //					   half_height_core ,
    //					   half_width_core ,
    //					   ( height_bounds - half_height_core ) / 2 ,
    //					   ( width_bounds - half_width_core ) / 2 );
    //	thread nd_thread ( sub_thread ,
    //					   ( height_bounds - half_height_core ) / 2 ,
    //					   half_height_core ,
    //					   height_bounds-half_height_core ,
    //					   ( width_bounds - half_width_core ) / 2 );
    //	thread rd_thread ( sub_thread ,
    //					   half_height_core ,
    //					   ( width_bounds - half_width_core ) / 2 ,
    //					   ( height_bounds - half_height_core ) / 2 ,
    //					   width_bounds - half_width_core );
    //	thread th_thread ( sub_thread ,
    //					   ( height_bounds - half_height_core ) / 2 ,
    //					   ( width_bounds - half_width_core ) / 2 ,
    //					   height_bounds - half_height_core ,
    //					   width_bounds - half_width_core );
    //	st_thread.join ( );
    //	nd_thread.join ( );
    //	rd_thread.join ( );
    //	th_thread.join ( );
    //	obj.SetComponentData ( dest ,component );
    //};
    //thread RedThread ( single_thread ,Red );
    //thread GreenThread ( single_thread ,Green );
    //thread BlueThread ( single_thread ,Blue );
    //RedThread.join ( );
    //GreenThread.join ( );
    //BlueThread.join ( );
    //double end = omp_get_wtime ( );
    //cout << "Execution time : " << end - start << endl;
    //obj.AddToFilename ( string ( "_average" ) ) ;
}
SobelOperator::SobelOperator ( )
{
    m_Radius = 1;
    m_Kernel.resize ( 3 , 3 );
    m_Kernel ( 0 , 0 ) = -1;
    m_Kernel ( 0 , 2 ) = 1;
    m_Kernel ( 1 , 0 ) = -2;
    m_Kernel ( 1 , 2 ) = 2;
    m_Kernel ( 2 , 0 ) = -1;
    m_Kernel ( 2 , 2 ) = 1;
    m_KernelY.resize ( 3 , 3 );
    m_KernelY ( 0 , 0 ) = -1;
    m_KernelY ( 0 , 1 ) = -2;
    m_KernelY ( 0 , 2 ) = -1;
    m_KernelY ( 2 , 0 ) = 1;
    m_KernelY ( 2 , 1 ) = 2;
    m_KernelY ( 2 , 2 ) = 1;
}
SobelOperator::~SobelOperator ( )
{
    m_KernelY.clear ( );
}
void SobelOperator::Apply ( CBMP &obj )
{
}
void SobelOperator::Apply ( CMatrix2D<int> &obj )
{
    int height_bounds = obj.GetHeight ( );
    int width_bounds = obj.GetWidth ( );
    int core_height_bounds = 2 * m_Radius + 1;
    int core_width_bounds = 2 * m_Radius + 1;
    int half_height_core = m_Radius + 1;
    int half_width_core = m_Radius + 1;
    assert ( height_bounds > 0 && width_bounds > 0 );
    float flSummaryX;
    float flSummaryY;
    CMatrix2D<int> dest ( obj );
    for ( auto j = half_height_core; j < height_bounds - half_height_core; j++ )
    {
        for ( auto k = half_width_core; k < width_bounds - half_width_core; k++ )
        {
            flSummaryX = 0;
            flSummaryY = 0;
            for ( auto y = 0; y < core_height_bounds; y++ )
            {
                for ( auto x = 0; x < core_width_bounds; x++ )
                {
                    flSummaryX += obj ( j + y - half_height_core , k + x - half_width_core )*m_Kernel ( y , x );
                    flSummaryY += obj ( j + y - half_height_core , k + x - half_width_core )*m_KernelY ( y , x );
                }
            }
            dest ( j , k ) = sqrt ( flSummaryX*flSummaryX + flSummaryY*flSummaryY );
            check_bounds ( dest ( j , k ) );
        }
    }
    obj = std::move ( dest );
}
LaplaceOperator::LaplaceOperator ( bool mode )
{
    m_Radius = 1;
    m_Kernel.resize ( 3 , 3 );
    if ( mode ) // Positive
    {
        m_Kernel ( 0 , 1 ) = 1;
        m_Kernel ( 1 , 0 ) = 1;
        m_Kernel ( 1 , 2 ) = 1;
        m_Kernel ( 2 , 1 ) = 1;
        m_Kernel ( 1 , 1 ) = -4;
    }
    else // Negative
    {
        m_Kernel ( 0 , 1 ) = -1;
        m_Kernel ( 1 , 0 ) = -1;
        m_Kernel ( 1 , 2 ) = -1;
        m_Kernel ( 2 , 1 ) = -1;
        m_Kernel ( 1 , 1 ) = 4;
    }
}
LaplaceOperator::~LaplaceOperator ( )
{
}
void LaplaceOperator::Apply ( CBMP & )
{
}
void LaplaceOperator::Apply ( CMatrix2D<int> &obj )
{
    int height_bounds = obj.GetHeight ( );
    int width_bounds = obj.GetWidth ( );
    int core_height_bounds = 2 * m_Radius + 1;
    int core_width_bounds = 2 * m_Radius + 1;
    int half_height_core = m_Radius + 1;
    int half_width_core = m_Radius + 1;
    assert ( height_bounds > 0 && width_bounds > 0 );
    float flSummary;
    CMatrix2D<int> dest ( obj );
    for ( auto j = half_height_core; j < height_bounds - half_height_core; j++ )
    {
        for ( auto k = half_width_core; k < width_bounds - half_width_core; k++ )
        {
            flSummary = 0;
            for ( auto y = 0; y < core_height_bounds; y++ )
            {
                for ( auto x = 0; x < core_width_bounds; x++ )
                {
                    flSummary += obj ( j + y - half_height_core , k + x - half_width_core )*m_Kernel ( y , x );
                }
            }
            dest ( j , k ) = flSummary;
            check_bounds ( dest ( j , k ) );
        }
    }
    obj = std::move ( dest );
}
