#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN                                                                                                      
    #include <doctest/doctest.h>                                                                                                                    
    #include "utils/PhysicsUtils.hpp"                                                                                                                  
                                                                                                                                                    
    TEST_SUITE("PhysicsUtils") {  
                                                                                                                                              
        TEST_CASE("Speed Calculation") {                                                                   
            SUBCASE("Normal movement: 0.2 meters in 40ms = 5.0 m/s") {                                     
                float speed = PhysicsUtils::CalculateSpeed(0.2f, 40);                                      
                CHECK(speed == doctest::Approx(5.0f));                                                     
            }                                                                                              
                                                                                                           
            SUBCASE("Time gap (> 50ms) resets speed to 0") {                                               
                float speed = PhysicsUtils::CalculateSpeed(0.2f, 100);                                     
                CHECK(speed == doctest::Approx(0.0f));                                                     
            }                                                                                              
                                                                                                           
            SUBCASE("Zero elapsed time returns 0") {                                                       
                float speed = PhysicsUtils::CalculateSpeed(0.2f, 0);                                       
                CHECK(speed == doctest::Approx(0.0f));                                                     
            }                                                                                              
        }                                                                                                  
                                                                                                           
        TEST_CASE("Acceleration Calculation") {                                                            
            SUBCASE("Speeding up: 2.0 m/s to 5.0 m/s over 40ms = 75.0 m/s²") {                             
                // (5.0 - 2.0) / 0.040s = 3.0 / 0.040 = 75 m/s²                                            
                float accel = PhysicsUtils::CalculateAcceleration(5.0f, 2.0f, 40);                         
                CHECK(accel == doctest::Approx(75.0f));                                                    
            }                                                                                              
                                                                                                           
            SUBCASE("Slowing down (Deceleration): 5.0 m/s to 2.0 m/s over 40ms = -75.0 m/s²") {            
                float accel = PhysicsUtils::CalculateAcceleration(2.0f, 5.0f, 40);                         
                CHECK(accel == doctest::Approx(-75.0f));                                                   
            }                                                                                              
                                                                                                           
            SUBCASE("Constant speed results in 0 acceleration") {                                          
                float accel = PhysicsUtils::CalculateAcceleration(5.0f, 5.0f, 40);                         
                CHECK(accel == doctest::Approx(0.0f));                                                     
            }                                                                                              
        }                                                                                                  
                 

        TEST_CASE("GetCartesianDistance calculates 2D Euclidean distance correctly") {                                                              
                                                                                                                                                    
            SUBCASE("Zero distance between identical points") {                                                                                     
                CHECK(PhysicsUtils::GetCartesianDistance(0.0f, 0.0f, 0.0f, 0.0f) == doctest::Approx(0.0f));                                              
                CHECK(PhysicsUtils::GetCartesianDistance(5.2f, -3.1f, 5.2f, -3.1f) == doctest::Approx(0.0f));                                            
            }                                                                                                                                       
                                                                                                                                                    
            SUBCASE("Horizontal displacement") {                                                                                                    
                CHECK(PhysicsUtils::GetCartesianDistance(1.0f, 2.0f, -4.0f, 2.0f) == doctest::Approx(5.0f));                                             
                CHECK(PhysicsUtils::GetCartesianDistance(1.0f, 2.0f, 4.0f, 2.0f) == doctest::Approx(3.0f));                                              
            }                                                                                                                                       
                                                                                                                                                    
            SUBCASE("Vertical displacement") {                                                                                                      
                CHECK(PhysicsUtils::GetCartesianDistance(2.0f, 1.0f, 2.0f, 5.0f) == doctest::Approx(4.0f));                                            
                CHECK(PhysicsUtils::GetCartesianDistance(2.0f, -1.0f, 2.0f, 5.0f) == doctest::Approx(6.0f));                                                
            }                                                                                                                                       
                                                                                                                                                    
            SUBCASE("Standard 3-4-5 right triangle") {                                                                                              
                CHECK(PhysicsUtils::GetCartesianDistance(0.0f, 0.0f, 3.0f, 4.0f) == doctest::Approx(5.0f));                                              
            }                                                                                                                                       
                                                                                                                                                    
            SUBCASE("Symmetry property (distance A to B equals B to A)") {
                float distAB = PhysicsUtils::GetCartesianDistance(-14.35f, 7.6f, 14.35f, -7.6f);
                float distBA = PhysicsUtils::GetCartesianDistance(14.35f, -7.6f, -14.35f, 7.6f);
                CHECK(distAB == doctest::Approx(distBA));
            }
        }
    }
