#ifndef WAGO_750_H
#define WAGO_750_H

#include "wago.h"

#include "kbusapi.h"

/// @brief
///
///
class wago_manager_w750 : public wago_manager
    {
    public:
        wago_manager_w750()
            {
            KbusOpen();
            }

        virtual ~wago_manager_w750()
            {
            KbusClose();
            }

        int read_inputs()
            {
            if ( 0 == nodes_count ) return 0;

                
            for ( u_int i = 0; i < nodes_count; i++ )
                {
                if ( nodes[ i ]->type == 0 ) // KBus
                    {
                    KbusUpdate();

                    // DI
                    int start_pos = KbusGetBinaryInputOffset() / 8;
#ifdef DEBUG
                    Print( "start_pos=%d\n", start_pos );
#endif // DEBUG
                    for ( u_int j = 0; j < nodes[ i ]->DI_cnt; j++ )
                        {
                        char tmp =  1 << ( j % 8 );

                        nodes[ i ]->DI[ j ] = ( ( tmp &
                            pstPabIN->uc.Pab[ start_pos + j / 8 ] ) > 0 );
#ifdef DEBUG
                        Print( "%d -> %d, ", j, nodes[ i ]->DI[ j ] );
#endif // DEBUG
                        }
#ifdef DEBUG
                    Print( "\n" );
#endif // DEBUG

                    // AI
                    for ( u_int j = 0; j < nodes[ i ]->AI_cnt; j++ )
                        {
                        float val = 0;
                        u_int offset = nodes[ i ]->AI_offsets[ j ];

                        switch ( nodes[ i ]->AI_types[ j ] )
                            {
                            case 461:
                                short int tmp = 256 * pstPabIN->uc.Pab[ offset ] +
                                    pstPabIN->uc.Pab[ offset + 1 ];
                                val = 0.1 * tmp;
                                
                                val = val >= -50 && val <= 150 ? val : -1000; 
                                break;

                            case 466:
                                val = 256 * pstPabIN->uc.Pab[ offset ] +
                                    pstPabIN->uc.Pab[ offset + 1 ];
                                break;

                            case 638:
                                val = 256 * pstPabIN->uc.Pab[ offset + 2 ] +
                                    pstPabIN->uc.Pab[ offset + 3 ];
                                break;
                            }
                        nodes[ i ]->AI[ j ] = val;
#ifdef DEBUG
                        Print( "%d -> %f\n, ", j, nodes[ i ]->AI[ j ] );
#endif // DEBUG
                        }
                    }
                }                
            
            return 0;
            }

        virtual int write_outputs()
            {
            if ( 0 == nodes_count ) return 0;

            for ( u_int i = 0; i < nodes_count; i++ )
                {
                if ( nodes[ i ]->type == 0 ) // KBus
                    {
                    KbusUpdate();

                    // DO
                    int start_pos = KbusGetBinaryOutputOffset() / 8;
#ifdef DEBUG
                    Print( "start_pos = %d\n", start_pos );
#endif // DEBUG
                    for ( u_int j = 0; j < nodes[ i ]->DI_cnt; j++ )
                        {
                        if ( nodes[ i ]->DO[ j ] != nodes[ i ]->DO_[ j ] )
                            {
                            char tmp =  1 << ( j % 8 );
                            if ( nodes[ i ]->DO_[ j ] )
                                {
                                pstPabOUT->uc.Pab[ start_pos + j / 8 ] |= tmp;
                                }
                             else
                                {
                                pstPabOUT->uc.Pab[ start_pos + j / 8 ] &= ~tmp;
                                }
                            nodes[ i ]->DO[ j ] = nodes[ i ]->DO_[ j ];
                            }
#ifdef DEBUG
                        Print( "%d -> %d, ", j, nodes[ i ]->DO_[ j ] );
#endif // DEBUG
                        }
#ifdef DEBUG
                    Print( "\n" );
#endif // DEBUG

                    // AO
                    for ( u_int j = 0; j < nodes[ i ]->AO_cnt; j++ )
                        {
                        if ( nodes[ i ]->AO[ j ] != nodes[ i ]->AO_[ j ] )
                            {
                            u_int val = ( u_int ) nodes[ i ]->AO_[ j ];
                            u_int offset = nodes[ i ]->AO_offsets[ j ];

                            switch ( nodes[ i ]->AO_types[ j ] )
                                {
                                case 554:
                                    pstPabIN->uc.Pab[ offset ] = val >> 8;
                                    pstPabIN->uc.Pab[ offset + 1 ] = val & 0xFF;
                                    break;
                                }
                            nodes[ i ]->AO[ j ] = nodes[ i ]->AO_[ j ];
                            }
#ifdef DEBUG
                        Print( "%d -> %f\n, ", j, nodes[ i ]->AO_[ j ] );
#endif // DEBUG
                        }
                    }
                }

            return 0;
            }
    };

#endif // WAGO_750_H
