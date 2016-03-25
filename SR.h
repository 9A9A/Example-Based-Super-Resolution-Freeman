#ifndef _SR_
#define _SR_
#include "Utilities.h"
#include "BMP.h"
#include <future>
#include <functional>
#include <mutex>

double Euclidean ( vector<float> &lhs , vector<float> &rhs )
{
    double value = 0;
    for ( size_t i = 0; i < lhs.size ( ); i++ )
    {
        value += ( lhs [ i ] - rhs [ i ] )*( lhs [ i ] - rhs [ i ] );
    }
    return sqrt ( value );
}
void convertImageToY ( BMP24& image )
{
    CMatrix2D<int> temp = image.GetComponentPixmap ( Y );
    image.SetComponentData ( temp , Red );
    image.SetComponentData ( temp , Green );
    image.SetComponentData ( temp , Blue );
}
void convertImageToCr ( BMP24& image )
{
    CMatrix2D<int> temp = image.GetComponentPixmap ( Cr );
    image.SetComponentData ( temp , Red );
    image.SetComponentData ( temp , Green );
    image.SetComponentData ( temp , Blue );
}
void smart_round ( double &x )
{
    long long temp = static_cast< long long > ( x );
    x = ( x - temp < 0.5f ) ? temp : temp + 1;
}
void smart_round ( float &x )
{
    long long temp = static_cast< long long > ( x );
    x = ( x - temp < 0.5f ) ? temp : temp + 1;
}
void clip ( CMatrix2D<float>& x )
{
    for ( auto &i : x )
    {
        smart_round ( i );
        check_bounds ( i );
    }
}
void clip ( CMatrix2D<rgbD>& x )
{
    for ( auto &i : x )
    {
        smart_round ( i.Red );
        smart_round ( i.Green );
        smart_round ( i.Blue );
        check_bounds ( i );
    }
}
void convertImageToCb ( BMP24& image )
{
    CMatrix2D<int> temp = image.GetComponentPixmap ( Cb );
    image.SetComponentData ( temp , Red );
    image.SetComponentData ( temp , Green );
    image.SetComponentData ( temp , Blue );
}
CMatrix2D<float> getComponent ( BMP24& image, int component )
{
    CMatrix2D<float> temp ( image.GetHeight ( ) , image.GetWidth ( ) );
    for ( size_t i = 0; i < temp.size ( ); i++ )
    {
        temp [ i ] = ( *image.GetInternal ( ) ) [ i ] [ component ];
    }
    return temp;
}
CMatrix2D<rgbD> fromYCbCr( CMatrix2D<float>& Y , CMatrix2D<float>& Cb , CMatrix2D<float>& Cr )
{
    CMatrix2D<rgbD> temp_patch ( Y.GetHeight ( ) , Y.GetWidth ( ) );
    for ( size_t i = 0; i < Y.size ( ); i++ )
    {
        temp_patch [ i ].Red = Y [ i ] + 1.402 * ( Cr [ i ] - 128 );
        temp_patch [ i ].Green = Y [ i ] - 0.34414*( Cb [ i ] - 128 ) - 0.71414*( Cr [ i ] - 128 );
        temp_patch [ i ].Blue = Y [ i ] + 1.772 * ( Cb [ i ] - 128 );
    }
    clip ( temp_patch );
    return temp_patch;
};
double mean ( CMatrix2D<float>& mtrx )
{
    double value = 0;
    for ( const auto &i : mtrx )
    {
        value += i;
    }
    return value / ( double ) mtrx.size ( );
}
double getscale ( CMatrix2D<float>& mtrx,double eps )
{
    return abs ( mean ( mtrx ) ) + eps;
}
class SR;
class SR_DB
{
    typedef CMatrix2D<float> patch;
    typedef pair<patch , patch> search_entry;
    typedef vector<search_entry> TrainingSet;
    string m_filename;
    const size_t sizetype = sizeof ( short );
    TrainingSet m_trainset;
    const size_t m_band_pass_size = 7;
    const size_t m_high_pass_size = 5;
    const size_t REPRESENTATION_SIZE = 1; // if using only Y in our prediction - size = 1, if RGB = 3;
    const size_t bandpass_size = m_band_pass_size*m_band_pass_size * REPRESENTATION_SIZE;
    const size_t highpass_size = m_high_pass_size*m_high_pass_size * REPRESENTATION_SIZE;
    const size_t entry_size = bandpass_size *sizetype + highpass_size*sizetype;
    const double alpha = 0.1 * m_band_pass_size*m_band_pass_size / ( 2 * m_high_pass_size - 1 );
public:
    friend class SR;
    SR_DB ( )
    {
    }
    SR_DB ( const string& filename )
    {
        load_from_drive ( filename );
    }
    void add_to_db ( BMP24& image , bool write_to_drive )
    {
        BMP24 temp_image ( image );
        convertImageToY ( temp_image );
        pair<patch , patch> AllFreqs = split_into_freqs ( temp_image );
        fstream handle;
        if ( write_to_drive )
        {
            if ( !m_filename.empty ( ) )
            {
                fstream filehandle ( m_filename , fstream::in | fstream::out | fstream::binary | fstream::app );
                handle = move ( filehandle );
            }
            else
            {
                throw runtime_error ( "SR_DB::add_to_db : super resoltuion file handle is not specified\n" );
            }
        }
        size_t db_size = m_trainset.size ( );
        size_t x = 0;
        for ( size_t i = 0; i < image.GetHeight ( ) - m_band_pass_size; i++ )
        {
            for ( size_t j = 0; j < image.GetWidth ( ) - m_band_pass_size; j++ )
            {
                x++;
                patch band_pass = AllFreqs.first.rect ( i , j , m_band_pass_size , m_band_pass_size );
                patch high_pass = AllFreqs.second.rect ( i + 1 , j + 1 , m_high_pass_size , m_high_pass_size );
                if ( write_to_drive )
                {
                    write_entry_to_drive ( handle , band_pass , high_pass );
                }
                add_entry_to_db ( band_pass , high_pass );
            }
        }
        cout << "Database size : " << db_size << " added : " << x << " -> " << m_trainset.size ( ) << "\n";
    }
    TrainingSet* data ( )
    {
        return &m_trainset;
    }
    void setname ( const string& filename )
    {
        m_filename = filename;
    }
    void print ( )
    {
        cout << "Database : " << m_trainset.size() << " search entries\n";
    }
    void load_from_drive ( const string& filename )
    {
        setname ( filename );
        fstream handle ( m_filename , fstream::in | fstream::out | fstream::binary );
        if ( handle.is_open ( ) )
        {
            cout << "Opened super-resolution database file : " << filename << endl;
            handle.seekg ( 0 , handle.end );
            size_t db_size = handle.tellg ( );
            handle.seekg ( handle.beg );
            if ( ( db_size % entry_size ) == 0 )
            {
                auto bandpass = new short [ bandpass_size ];
                auto highpass = new short [ highpass_size ];
                patch high_pass ( m_high_pass_size , m_high_pass_size );
                patch band_pass ( m_band_pass_size , m_band_pass_size );
                for ( size_t i = 0; i < db_size; i += entry_size )
                {
                    handle.read ( reinterpret_cast< char* >( bandpass ) , bandpass_size*sizetype );
                    handle.read ( reinterpret_cast< char* >( highpass ) , highpass_size*sizetype );
                    buffer_to_patch ( band_pass , bandpass , 0 , bandpass_size );
                    buffer_to_patch ( high_pass , highpass , 0 , highpass_size );
                    add_entry_to_db ( band_pass , high_pass );
                }
                delete [ ] bandpass;
                delete [ ] highpass;
            }
            else
            {
                throw runtime_error ( "incorrect db file\n" );
            }
            print ( );
        }
        else
        {
            cout << "No such file\n";
        }
    }
private:
    void add_entry_to_db ( patch& band_pass , patch& high_pass )
    {
        patch search_vec = build_search_vector ( band_pass , high_pass );
        float scale = getscale ( search_vec , 0.0001 );
        search_vec = search_vec / scale;
        high_pass = high_pass / scale;
        m_trainset.emplace_back ( search_entry ( search_vec , high_pass ) );
    }
    void buffer_to_patch ( patch& dst , short* src , size_t st_pos , size_t length ) const
    {
        size_t index = 0;
        for ( size_t i = st_pos; i < length; i++ )
        {
            dst [ index++ ] = src [ i ];
        }
    }
    patch build_search_vector ( patch& band_pass , patch& high_pass ) const
    {
        patch search_vec ( 1 , 58 );
        size_t i;
        for ( i = 0; i < band_pass.size ( ); i++ )
        {
            search_vec [ i ] = band_pass [ i ];
        }
        vector<float> first_column = high_pass.get_column ( 0 );
        vector<float> first_row = high_pass.get_row ( 0 );
        first_column.insert ( first_column.end ( ) , first_row.begin ( ) + 1 , first_row.end ( ) );
        for ( auto &it : first_column )
        {
            search_vec [ i++ ] = it*alpha;
        }
        return search_vec;
    }
    short* patch_to_buffer ( patch& src ) const
    {
        auto buf = new short [ src.size ( ) ];
        for ( size_t i = 0; i < src.size ( ); i++ )
        {
            buf [ i ] = src [ i ];
        }
        return buf;
    }
    pair<patch , patch> split_into_freqs ( BMP24& all_freqs )
    {
        BMP24 lowFreqs ( all_freqs );
        // subsampling and restoring to same resolution 
        lowFreqs.SetPixelData ( InterpolateBicubic ( lowFreqs.GetPixelData ( ) , 0.5 , 0.5 ) );
        lowFreqs.SetPixelData ( InterpolateBicubic ( lowFreqs.GetPixelData ( ) , 2 , 2 ) );
        BMP24 highFreqs ( all_freqs );
        highFreqs.SetPixelData ( *all_freqs.GetInternal ( ) - *lowFreqs.GetInternal ( ) );
        return pair<patch , patch> ( getComponent ( lowFreqs , Red ) , getComponent ( highFreqs , Red ) );
    }
    void write_entry_to_drive ( fstream& handle , patch& bandpass , patch& highpass )
    {
        auto low = patch_to_buffer ( bandpass );
        auto high = patch_to_buffer ( highpass );
        handle.write ( reinterpret_cast< char* >( low ) , bandpass.size ( ) * REPRESENTATION_SIZE * sizetype );
        handle.write ( reinterpret_cast< char* >( high ) , highpass.size ( ) * REPRESENTATION_SIZE * sizetype );
        delete [ ] low;
        delete [ ] high;
    }
};
class SR
{
    shared_ptr<SR_DB> m_pdb;
public:
    SR ( )
    {

    }
    SR ( shared_ptr<SR_DB> db ) : m_pdb ( db )
    {

    }
    void process ( BMP24& image , double scale )
    {
        BMP24 original_image ( image );
        BMP24 interpolated_image ( image );
        SR_DB::patch OriginalY = original_image.GetComponentData ( Y );
        interpolated_image.SetPixelData ( InterpolateBicubic ( interpolated_image.GetPixelData ( ) , scale , scale ) );
        SR_DB::patch interpolatedY = interpolated_image.GetComponentData ( Y );
        SR_DB::patch HighFreqY ( interpolated_image.GetHeight ( ) , interpolated_image.GetWidth ( ) , 0.0 );
        size_t query = 0;
        for ( size_t i = 0; i < HighFreqY.GetHeight ( ) - 6; i += 4 ) { for ( size_t j = 0; j < HighFreqY.GetWidth ( ) - 6; j += 4 , query++ ){}}
        size_t done = 0;
        double average_time = 0;
        for ( size_t i = 0; i < HighFreqY.GetHeight ( ) - 6; i += 4 )
        {
            for ( size_t j = 0; j < HighFreqY.GetWidth ( ) - 6; j += 4 )
            {
                auto loopstart = high_resolution_clock::now ( );
                SR_DB::patch band_pass = interpolatedY.rect ( i , j , m_pdb->m_band_pass_size , m_pdb->m_band_pass_size );
                SR_DB::patch high_pass = HighFreqY.rect ( i + 1 , j + 1 , m_pdb->m_high_pass_size , m_pdb->m_high_pass_size );
                SR_DB::patch search_key = m_pdb->build_search_vector ( band_pass , high_pass );

                float scale = getscale ( search_key , 0.0001 );
                search_key = search_key / scale;

                SR_DB::patch search_result = search ( search_key );

                search_result = search_result * scale;
                // just replace or append?!
                HighFreqY.emplace ( search_result , i + 1 , j + 1 );

                auto loopend = high_resolution_clock::now ( );
                done++;
                average_time += ( duration_cast< microseconds >( loopend.time_since_epoch ( ) - loopstart.time_since_epoch ( ) ).count ( ) / 1000000.0f );
                cout.precision ( 4 );
                cout << "\r\rProcess : " << 100.0 *( double ) done / ( double ) query << " % Estimated time remaining : " << ( average_time / done ) *( query - done ) << " sec                 ";
            }
        }
        HighFreqY = HighFreqY + interpolatedY;
        clip ( HighFreqY );
        interpolated_image.AddToFilename ( "_INTERPOLATED_BICUBIC" );
        interpolated_image.SaveFile ( );
        interpolated_image.SetPixelData ( fromYCbCr ( HighFreqY , interpolated_image.GetComponentData ( Cb ) , interpolated_image.GetComponentData ( Cr ) ) );
        interpolated_image.SaveFile ( image.GetFilename ( ) + "_SUPERRES" );
    }
private:
    SR_DB::patch search ( SR_DB::patch &fs )
    {
        auto search_sub = [ & ] ( size_t start , size_t end , SR_DB::patch& src )->pair<size_t , double>
        {
            pair<size_t , double > x;
            x.first = 0;
            x.second = INT64_MAX;
            size_t limit = ( end >= m_pdb->data ( )->size ( ) ) ? m_pdb->data ( )->size ( ) : end;
            for ( size_t i = start; i < limit; i++ )
            {
                double metrics = Euclidean ( fs.data ( ) , ( *m_pdb->data ( ) ) [ i ].first.data ( ) );
                if ( metrics < x.second )
                {
                    x.second = metrics;
                    x.first = i;
                }
            }
            return x;
        };
        vector<future<pair<size_t , double>>> futures;
        vector<pair<size_t , double>> future_results;
        size_t part = m_pdb->data ( )->size ( ) / thread_amount;
        for ( auto i = 0; i < thread_amount; i++ )
        {
            futures.push_back ( async ( search_sub , part*i , part*( i + 1 ) , fs ) );
        }
        for ( auto &i : futures )
        {
            future_results.push_back ( i.get ( ) );
        }
        static size_t calls = 0;
        pair<size_t , double > total_result;
        total_result.first = 0;
        total_result.second = INT64_MAX;
        for ( auto &i : future_results )
        {
            if ( total_result.second > i.second )
            {
                total_result.second = i.second;
                total_result.first = i.first;
            }
        }
        return ( *m_pdb->data ( ) ) [ total_result.first ].second;

    }

};

#endif