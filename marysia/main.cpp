#include <iostream>

#include "stdint.h"
#include <time.h>
#include <stdlib.h>
#include "table/Row.h"
#include "buffer/Buffer.h"
#include "filestruct/TableManager.h"
#include "table/columns/ColumnVarchar.h"
#include "table/columns/ColumnInteger.h"
#include "table/columns/ColumnTinyint.h"
#include "table/columns/ColumnDatetime.h"
#include "table/columns/ColumnDouble.h"
#include "utils/RandGenerator.h"
#include "table/values/ValueVarchar.h"
#include "table/values/ValueInteger.h"
#include "table/values/ValueTinyint.h"
#include "table/values/ValueDatetime.h"
#include <stdio.h>
#include "table/values/ValueDouble.h"
#include "table/KeyValue.h"
#include "table/columns/ColumnBlob.h"
#include "utils/ByteData.h"
#include "table/values/ValueBlob.h"

using namespace std;

int main(int argc, char **argv) {
    srand((unsigned)time(NULL));

    remove("uzytkownicy.bin");
    remove("uzytkownicy.inf");
    
    //cout << sysconf(_SC_PAGE_SIZE) << endl;
    
    TableManager* tablemanager = new TableManager( );

    tablemanager->createTable( "uzytkownicy" );
    
    tablemanager->addColumn( new ColumnVarchar( 
                            "user_name",
                             16,
                             true)
    );

    tablemanager->addColumn( new ColumnDouble( 
                            "rate", 
                            false, 
                            3.2 ) 
    );
    
    tablemanager->addColumn( new ColumnInteger( 
                            "user_id",
                            false,
                            false)
    );
    
    tablemanager->addColumn( new ColumnTinyint( 
                            "user_age",
                            false,
                            false)
    );
    
    tablemanager->addColumn( new ColumnVarchar( 
                            "city",
                             16,
                             true,
                             "lublin" )
    );
    
    tablemanager->addColumn( new ColumnDatetime( 
                            "birth_date", 
                            false ) 
    );
    
    tablemanager->addColumn( new ColumnBlob( 
                            "picture", 
                            false ) 
    );

    tablemanager->addKey( "userid", "user_name", true, true );

    tablemanager->closeTable();

    tablemanager->openTable( "uzytkownicy" );

    vector<String> vec;
    uint32_t rowNum = 10000;

    for (int i = 1; i <= rowNum; i+=1)
    {
        String key = RandGenerator::randomString( 6 );
        vec.push_back( key );
        
        uint32_t size = 16120*2;
        uint8_t* blob = new uint8_t[size];
        ByteData::dataMemSet( blob, 77, size );

        if ( ! tablemanager->insertRow( & ( *( new Row() )
                                            << new ValueVarchar( key.c_str() )
                                            << new ValueDouble( ( double ) RandGenerator::randomInt( 10 ) / 10.0 )
                                            << new ValueInteger( RandGenerator::randomInt( 4 ) )
                                            //<< new ValueInteger( )
                                            << new ValueTinyint( RandGenerator::randomInt( 4 ) )
                                            << new ValueVarchar( "Lublin" )
                                            << new ValueDatetime( "2009-03-13 13:33:20" )
                                            << new ValueBlob( blob, size )
                                            ) ) == true )
        {
            cout << "row " << i << " already exists" << endl;
        }
        
        delete [] blob;
    }

    Row* row;
    tablemanager->readInit("userid");
    int n = 0;
    while ( ( row = tablemanager->readNext() ) != NULL )
    {
        cout /*<< ++n*/ << " " << *row << endl;
        delete row;
    }

    tablemanager->readClose();

    for (int i = 1; i <= rowNum; i+=1)
    {
        String key = vec.back();
        vec.pop_back();

        tablemanager->deleteRow( & ( *( new KeyValue() )
                                     << new ValueVarchar( key.c_str() ) ) );
    }

    tablemanager->readInit( "userid" );

    int i = 0;
    while ( ( row = tablemanager->readNext() ) != NULL )
    {
        cout << ++i << *row << endl;
        delete row;
    }

    tablemanager->readClose();

    tablemanager->closeTable();

    delete tablemanager;

    return 0;
}
