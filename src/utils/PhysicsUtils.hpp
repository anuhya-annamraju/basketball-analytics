#pragma once                                                                                           
    #include <cmath>                                                                                       
                                                                                                           
    namespace PhysicsUtils {                                                                               
                                                                                                           
        // Calculates speed in m/s given distance (meters) and elapsed time (milliseconds)                 
        inline float CalculateSpeed(float step_distance_m, long long dt_ms) {                                   
            if (dt_ms <= 0 || dt_ms > 50) {                                                                
                return 0.0f; // Gap or invalid timestamp                                                   
            }                                                                                              
            float dt_sec = dt_ms * 0.001f;                                                                 
            return step_distance_m / dt_sec;                                                                    
        }                                                                                                  
                                                                                                           
        // Calculates acceleration in m/s² given current speed, previous speed, and elapsed time (ms)      
        inline float CalculateAcceleration(float currentSpeed_ms, float previousSpeed_ms, long long dt_ms) 
  {                                                                                                        
            if (dt_ms <= 0 || dt_ms > 50) {                                                                
                return 0.0f; // Gap or invalid timestamp                                                   
            }                                                                                              
            float dt_sec = dt_ms * 0.001f;                                                                 
            return (currentSpeed_ms - previousSpeed_ms) / dt_sec;                                          
        }                                                                                                  
    
        inline float GetCartesianDistance(float x1, float y1, float x2, float y2)
        {                                      
            float dx = x2 - x1;                                                                                              
            float dy = y2 - y1;                                                                                              
            float distance = std::hypot(dx, dy);  
            return distance;
        }

    }                 