#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN                                                                                                      
    #include <doctest/doctest.h>                                                                                                                    
    #include "SnippetAnalyser.hpp"                                                                                                                  
                                                                                                                                                    
    TEST_SUITE("SnippetAnalyser") {                                                                                                                 
        TEST_CASE("GetCartesianDistance calculates 2D Euclidean distance correctly") {                                                              
            SnippetAnalyser analyser;                                                                                                               
                                                                                                                                                    
            SUBCASE("Zero distance between identical points") {                                                                                     
                CHECK(analyser.GetCartesianDistance(0.0f, 0.0f, 0.0f, 0.0f) == doctest::Approx(0.0f));                                              
                CHECK(analyser.GetCartesianDistance(5.2f, -3.1f, 5.2f, -3.1f) == doctest::Approx(0.0f));                                            
            }                                                                                                                                       
                                                                                                                                                    
            SUBCASE("Horizontal displacement") {                                                                                                    
                CHECK(analyser.GetCartesianDistance(1.0f, 2.0f, -4.0f, 2.0f) == doctest::Approx(5.0f));                                             
                CHECK(analyser.GetCartesianDistance(1.0f, 2.0f, 4.0f, 2.0f) == doctest::Approx(3.0f));                                              
            }                                                                                                                                       
                                                                                                                                                    
            SUBCASE("Vertical displacement") {                                                                                                      
                CHECK(analyser.GetCartesianDistance(2.0f, 1.0f, 2.0f, 5.0f) == doctest::Approx(4.0f));                                            
                CHECK(analyser.GetCartesianDistance(2.0f, -1.0f, 2.0f, 5.0f) == doctest::Approx(6.0f));                                                
            }                                                                                                                                       
                                                                                                                                                    
            SUBCASE("Standard 3-4-5 right triangle") {                                                                                              
                CHECK(analyser.GetCartesianDistance(0.0f, 0.0f, 3.0f, 4.0f) == doctest::Approx(5.0f));                                              
            }                                                                                                                                       
                                                                                                                                                    
            SUBCASE("Symmetry property (distance A to B equals B to A)") {
                float distAB = analyser.GetCartesianDistance(-14.35f, 7.6f, 14.35f, -7.6f);
                float distBA = analyser.GetCartesianDistance(14.35f, -7.6f, -14.35f, 7.6f);
                CHECK(distAB == doctest::Approx(distBA));
            }
        }
    }
