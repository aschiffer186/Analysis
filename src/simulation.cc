#include "simulation.hh"
#include "simulator.hh"
#include "statistics.hh"

#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>

bool strequal(const std::string& a, const std::string& b)
{
    return std::equal(a.begin(), a.end(), b.begin(), [](char a, char b){return toupper(a) == toupper(b);});
}

void calculate_statistics(const std::vector<double>& data)
{
    double avg = mean(data.begin(), data.end());
    double s_deviation = stdev(data.begin(), data.end());
    double min = *std::min_element(data.begin(),data.end());
    double max = *std::max_element(data.begin(), data.end());

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average: $" <<avg << std::endl;
    std::cout << "Maximum: $" << max << std::endl;
    std::cout << "Minimum: $" << min << std::endl;
    std::cout << "Standard Deviation: $" << s_deviation << std::endl;
    std::cout << "One Standard Deviation Above the Mean: $" << avg + s_deviation << std::endl;
    std::cout << "One Standard Deviation Below the Mean: $" << avg - s_deviation << std::endl;
    std::cout << std::endl;
}

void simulation_main()
{
    std::string user_input = "";
    do 
    {
        std::cout << "Select from the following options\n";
        std::cout << "[1] Overall Fundraising Prediction\n";
        std::cout << "[2] Giving Day Fundraising Prediction\n";
        std::cout << "[3] VictorThon Fundraising Prediction\n";
        std::cout << "[4] Quit\n";
        std::cout << ">> ";
        std::cin >> user_input;

        if(user_input.find("1") != std::string::npos)
        {
            std::cout << "To be implemented: " << std::endl;
        }
        else if(user_input.find("2") != std::string::npos)
        {
            giving_day_prediction();
        } else if (user_input.find("3") != std::string::npos)
        {
            std::cout << "To be implemented: " << std::endl;
        }
    } while (!strequal(user_input, "quit") && !strequal(user_input, "4"));
}

int get_int_value()
{
    while(true)
    {
        std::string input;
        std::cin >> input;
        int num_members;
        try 
        {
            num_members = std::stoi(input);
            if (num_members <= 0)
                std::cout << "\nValue must be a positive number; please enter a new value: ";
            else
                return num_members;
        } catch (...) {
            std::cout << "\nInvalid Input; please enter a new value: ";
        }
    }
}

double get_engagement_percent()
{
    while(true)
    {
        std::string input;
        std::cin >> input;
        double engagement;
        try 
        {
            engagement = std::stod(input);
            if (engagement < 0 || engagement > 100)
                std::cout << "\nEngagement percentage must be between 0 and 100; please enter a new value: ";
            else
                return engagement/100;
        } catch (...) {
            std::cout << "\nInvalid input, please enter a new value: ";
        }
    }
}

double get_max_matching()
{
    while(true)
    {
        std::string input;
        std::cin.ignore();
        if (std::cin.peek() == '\n')
            return static_cast<double>(std::numeric_limits<int>::max());
        std::cin >> input;
        size_t index = input.find("$");
        if (index != std::string::npos)
            input = input.substr(index + 1);
        double max_amt;
        try
        {
            max_amt = std::stod(input);   
            if (max_amt < 0)
                std::cout << "Maximum amount someone can be matched must be non-negative; pleae enter a new value: ";
            else
                return max_amt;
        } catch (...) {
            std::cout << "\nInvalid input, please enter a new value: ";
        }
    }
}

auto matching_func = [](double amt_raised, double r, double max_amt)
{
    if (amt_raised >= max_amt)
    {
        if (r <= 0.8)
            return max_amt;
        else
            return 25.0;
    } 
    else 
    {
        if (amt_raised < 25)
            return amt_raised;
        else 
        {
            if (r <= 0.8)
                return amt_raised;
            else 
                return 25.0;
        }
    }
};

void giving_day_prediction()
{
    #ifdef ANALYSIS_PROJECT_INSTALLED
        std::string prefix = "../model/";
    #else
        std::string prefix = "models/";
    #endif

    std::string run_again = "";

    do 
    {
        std::cout << "Select Fundraising Model\n";
        std::cout << "[1] Pandemic Model\n";
        std::cout << "[2] 2019 Model\n";
        std::cout << "[3] 2018 Model\n";
        std::cout << "[4] Non-Pandemic Average\n";
        std::cout << "[5] Three Year Average\n";
        std::cout << ">> ";

        std::string input;
        std::cin >> input;

        std::string dancer_model = prefix;
        std::string leadership_model = prefix;

        if(input.find("1") != std::string::npos)
        {
            dancer_model += "2020d.txt";
            leadership_model += "2020l.txt";
        } 
        else if (input.find("2") != std::string::npos)
        {
            dancer_model += "2019d.txt";
            leadership_model += "2019l.txt";
        } 
        else if (input.find("3") != std::string::npos)
        {
            std::cout << "To be implemented soon: " << std::endl;
            continue;
        } 
        else if (input.find("4") != std::string::npos)
        {
            dancer_model += "npd.txt";
            leadership_model += "npl.txt";
        }
        else if (input.find("5") != std::string::npos)
        {
            std::cout << "To be implemented soon:" << std::endl;
            continue;
        }

        int num_leadership, num_dancers, num_simulations;
        double max_matching_amt, leadership_engagement, dancer_engagement;

        std::cout << "Enter number of leadership members: ";
        num_leadership = get_int_value();

        std::cout << "Enter number of dancers: ";
        num_dancers = get_int_value();

        std::cout << "Enter expected leadership engagement percentage (0 - 100): ";
        leadership_engagement = get_engagement_percent();

        std::cout << "Enter expected dancer engagement percentage (0 - 100): ";
        dancer_engagement = get_engagement_percent();

        std::cout << "Enter the maximum amount someone can be matched (or press <Enter> for unlimited matching): ";
        max_matching_amt = get_max_matching();

        std::cout << "Enter the number of simulations (recommended value: 500): ";
        num_simulations = get_int_value();

        std::vector<double> dancer_data;
        std::vector<double> leadership_data;

        std::ifstream dfin(dancer_model.c_str());
        std::ifstream lfin(leadership_model.c_str());

        double d;
        while(dfin >> d)
            dancer_data.push_back(d);
        while (lfin >> d)
            leadership_data.push_back(d);
        
        dfin.close(); lfin.close();

        simulator l_sim(leadership_data); simulator d_sim(dancer_data);
        auto leadership_gtd = l_sim.simulate(num_simulations, round(num_leadership*leadership_engagement), matching_func, max_matching_amt);
        auto dancer_gtd = d_sim.simulate(num_simulations, round(num_dancers*dancer_engagement), matching_func, max_matching_amt);

        //Leadership stats
        auto leadership_f = leadership_gtd[0];
        auto leadership_m = leadership_gtd[1];

        //Dancer stats
        auto dancer_f = dancer_gtd[0];
        auto dancer_m = dancer_gtd[1];

        std::vector<double> overall_f(num_simulations);
        std::vector<double> overall_m(num_simulations);

        for(size_t i = 0; i < num_simulations; ++i)
        {
            overall_f[i] = leadership_f[i] + leadership_f[i];
            overall_m[i] = leadership_m[i] + dancer_m[i];
        }

        std::cout << std::endl;
        std::cout << "Leadership Fundraising Statistics" << std::endl;
        calculate_statistics(leadership_f);
        
        std::cout << "Leadership Matching Money Used Statistics" << std::endl;
        calculate_statistics(leadership_m);

        std::cout << "Dancer Fundraising Statistics" << std::endl;;
        calculate_statistics(dancer_f);
        
        std::cout << "Dancer Matching Money Used Statistics" << std::endl;
        calculate_statistics(dancer_m);

        std::cout << "Org-Wide Fundraising Statistics" << std::endl;
        calculate_statistics(overall_f);
        
        std::cout << "Org-Wide Matching Money Used Statistics" << std::endl;
        calculate_statistics(overall_m);

        std::cout << "Run another simulation? (Y/N) ";
        std::cin >> run_again;
    } while (!strequal(run_again, "N"));
}
