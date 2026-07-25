#include <vector>
#include <iostream>


struct PlayerAnalytics {
    float distance = 0.0f;
    float speed;
    float acceleration;

    float intermediateDistance = 0.0f;
};

struct SnippetAnalytics
{
    std::vector<std::tuple<int,PlayerAnalytics>> player_analytics;
};

class SnippetAnalyser {
public:
SnippetAnalyser(){
    std::cout << "SnippetAnalyser object created." << std::endl;
}
~SnippetAnalyser(){}

void AnalyseSnippet(){}

private:

     void CalculateDistance();
  };
