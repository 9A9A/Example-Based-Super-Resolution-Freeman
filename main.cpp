#include "BMP.h"
#include "Filters.h"
#include <chrono>
#include "SR.h"
using namespace chrono;
int main ( )
{
	try
	{
		auto db = shared_ptr<SR_DB> ( new SR_DB );
		string x;
		cout << "db name > ";
		getline ( cin , x );
		db->load_from_drive ( x );
		while ( true )
		{
			cout << "file to add > ";
			getline ( cin , x );
			if ( x.empty ( ) )
			{
				break;
			}
			else
			{
				BMP24 temp ( x );
				db->add_to_db ( temp , true );
			}
			x.clear ( );
		}
		while ( true )
		{
			cout << "image to enlarge > ";
			getline ( cin , x );
			if ( x.empty ( ) )
			{
				break;
			}
			else
			{
				BMP24 temp ( x );
				high_resolution_clock::time_point x2 = high_resolution_clock::now ( );
				SR super_res ( db );
                super_res.process ( temp , 2 );
				cout.precision ( 10 );
				high_resolution_clock::time_point x3 = high_resolution_clock::now ( );
				cout << "\nTime elapsed : " << duration_cast< milliseconds >( x3.time_since_epoch ( ) - x2.time_since_epoch ( ) ).count ( ) / 1000.0 << " sec\n";
				break;
			}
			x.clear ( );
		}
	}
	catch ( runtime_error &err )
	{
		cout << err.what ( ) << endl;
	}
	system ( "pause" );

}