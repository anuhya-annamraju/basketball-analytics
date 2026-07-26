#include "SnippetAnalyser.hpp"


void SnippetAnalyser::CalculateDistance(){}

float SnippetAnalyser:: GetCartesianDistance(float x1, float y1, float x2, float y2)
{                                      
    float dx = x2 - x1;                                                                                              
    float dy = y2 - y1;                                                                                              
    float distance = std::hypot(dx, dy);  
    return distance;
}

float SnippetAnalyser::GetCartesianSpeed(float distance, float dt)
{
    return distance/dt;
}