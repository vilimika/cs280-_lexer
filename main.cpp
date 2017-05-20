#include <iostream>
#include <string>
#include <cstdlib>
#include <list>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include "polylex.h"
#include "token_strings.h"

using namespace std;

//tokenizing function
TokenTypes get_token(istream *, string&, int&);
TokenTypes string_recognize(istream*, string&, char&);
TokenTypes int_recognize(istream*, string&, char&);
TokenTypes float_recognize(istream*, string&, char&);
TokenTypes id_recognize(istream *, string &, char &);
//error functions
void many_file_error();
void bad_arg_error(string);
void bad_file_error(string);
int many_file_check(string, bool&, list<string>&);
//line skip function
void skip_comment_line(istream*);
//output function
void output_sum(map<TokenTypes, int>&, int);
void output_v(map<TokenTypes, list<string>>&, vector<string>&);
void output_ERR(int, string);
void output_stats(map<TokenTypes, int>, map<TokenTypes, list<string>>, vector<string>);
//- flag check functon
void flag_check(string, bool&, bool&, bool&, bool&);
//making string vector
void make_most_freq_num (map<TokenTypes, int>, vector<string>&);
void make_str_vec (vector<string>&, string, TokenTypes);
int make_id_vec (vector<string>&, string);



int main(int argc, char *argv[])
{
    //variables
    int currentLine = 0;    //extern variable to keep line count
    ifstream infile;
    list<string> file_list;
    istream *from = &cin;
    //flag checks
    bool v_flag = false;
    bool sum_flag = false;
    bool stats_flag = false;
    bool incorrect_arg = false;     //flag to close program if an incorrect argument was recognized
    bool many_file = false;         //flag to close program if too many files were passed
    //token containers
    string current_lexeme;
    list<string> lexemes;
    map<TokenTypes, int> counter;    //map to keep count of tokens
    map<TokenTypes, list<string>> token_map;    //map to keep all found tokens
    vector<string> t_str;
    vector<string> id_vec;
    //end variables

    //loop that takes care of -flags
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            string str (argv[i]);
            flag_check(str, incorrect_arg, sum_flag, v_flag, stats_flag);
            if(incorrect_arg == true)
            {
                bad_arg_error(str);
            }
        }
    }
    //end of -flag recognizing loop

   //file check loop begins
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            string str (argv[i]);
            many_file_check(str, many_file, file_list);

        }
    }
    //end file check loop

    if(many_file == true)
    {
        many_file_error();
    }

    //initiate tokenization

    if(file_list.size() == 0)
    {
        file_list.push_back("");
    }
    else
    {
        string file = file_list.front();
        file = file.c_str();
        infile.open(file);
        from = &infile;
        if(!infile.is_open())
            bad_file_error(file);
    }
    //check if file is valid

    TokenTypes cur;
    while(from -> good())
    {
        current_lexeme = "";
        cur = get_token(from, current_lexeme, currentLine);
        if(cur != TokenTypes :: DONE)
        {
            counter[cur]++;
            token_map[cur].push_back(current_lexeme);
            make_str_vec (t_str, current_lexeme, cur);
            if(cur == TokenTypes :: ID)
                make_id_vec (id_vec, current_lexeme);
        }
        if(cur == TokenTypes :: ERR)
        {
            output_ERR(currentLine, current_lexeme);
            exit(0);
        }
    }
        //output call
     if(v_flag == true)
        output_v(token_map, t_str);       //THIS NEEDS TO BE FINISHED
     if(stats_flag == true)
        output_stats(counter, token_map, id_vec);
     if(sum_flag == true)
        output_sum(counter, currentLine);


    return 0;
}


//main tokenizing function
TokenTypes get_token(istream *s, string &cur_lex, int &currentLine)
{
    char ch;
    s -> get(ch);
    if(s -> eof())
    {
        return TokenTypes :: DONE;
    }
    if(ch == '\n')
        currentLine++;
    //while loop that steps thru spaces
    while((ch == ' ' || ch == '\t' || ch == '\n') && !s -> eof())
    {
        s -> get(ch);
        if(ch == '\n' && s -> peek() == '\n')
            currentLine++;
        else if(ch == '\n' && !(s -> eof()))
            currentLine++;
    }

    if(ch == '#')
    {
        skip_comment_line(s);
        currentLine++;
    }
    if(ch == '.' && isdigit(s -> peek()))
    {
        cur_lex += ch;
        return TokenTypes :: ERR;
    }
    if(ch == '(')
        return TokenTypes :: LPAREN;
    if(ch == ')')
        return TokenTypes :: RPAREN;
    if(ch == '[')
        return TokenTypes :: LSQ;
    if(ch == ']')
        return TokenTypes :: RSQ;
    if(ch == '{')
        return TokenTypes :: LBR;
    if(ch == '}')
        return TokenTypes :: RBR;
    if(ch == '+')
        return TokenTypes :: PLUS;
    if(ch == '-')
        return TokenTypes :: MINUS;
    if(ch == '*')
        return TokenTypes :: STAR;
    if(ch == ',')
        return TokenTypes :: COMMA;
    if(ch == ';')
        return TokenTypes :: SC;
    if(isdigit(ch))
    {   TokenTypes return_token;
        return_token = int_recognize(s, cur_lex, ch);
        return return_token;
    }
    if(isalpha(ch))
    {   TokenTypes return_token;
        return_token = id_recognize(s, cur_lex, ch);
        return return_token;
    }
    if(ch == '"')
    {
        TokenTypes return_token;
        return_token = string_recognize(s, cur_lex, ch);
        return return_token;
    }

        return TokenTypes :: DONE;

}

//id tokenizing function
TokenTypes id_recognize(istream *s, string &word, char &ch)
{
    word += ch;
    while(s -> peek() != ' ' && s -> peek() != '\n' && s -> peek() != ',' && s -> peek() != ')' && s -> peek() != '}' && s -> peek() != ']' && s -> peek() != '.' && s -> peek() != '-' && s -> peek() != ';' && s -> peek() != '(' && s -> peek() != '[' && s -> peek() != '{')
    {
        s -> get(ch);
        word += ch;
    }
    return TokenTypes :: ID;
}

//string tokenizing function
TokenTypes string_recognize(istream *s, string &word, char &ch)
{
    //WORD VARIABLE IS CURRENT_LEXEME IN MAIN
    word += ch;
    while(s -> peek() != '"')
    {
        if(ch == '\n')
        {
            return TokenTypes :: ERR;
        }
        s -> get(ch);
        if(ch != '\n')
            word += ch;
    }
    if(s -> peek() == '"')
        s -> get(ch);
    word += ch;
    return TokenTypes :: STRING;
}

//number tokenizing function
TokenTypes int_recognize(istream *s, string &word, char &ch)
{
    TokenTypes return_token;
    word += ch;
    while(!(isspace(s -> peek())) && s -> peek() != '-')
    {
        s -> get(ch);
        if(ch == '.')
        {
            return_token = float_recognize(s, word, ch);
            return return_token;
        }
        word += ch;
    }
    return TokenTypes :: ICONST;
}

//float tokenizing function
TokenTypes float_recognize(istream *s, string &word, char &ch)
{
    word += ch;
    while(s -> peek() != ' ' && s -> peek() != '\n')
    {
        s -> get(ch);
        word += ch;
    }
    return TokenTypes :: FCONST;
}

//function handling - flags
void flag_check(string current_arg, bool &close_flag, bool &sum, bool &v, bool &stats)
{
    bool trigger = true;
    if(current_arg == "-v")
    {
        v = true;
        trigger = false;
    }
    if(current_arg == "-stats")
    {
        stats = true;
        trigger = false;
    }
    if(current_arg == "-sum")
    {
        sum = true;
        trigger = false;
    }
    if(trigger == true)
        close_flag = true;
}


//function handling file checks
int many_file_check(string current_file, bool &many_flag, list<string> &f_list)
{
    f_list.push_back(current_file);

    if(f_list.size() > 1)
    {
        many_flag = true;
        return 0;
    }
    return 0;
}

//bad - argument error
void bad_arg_error(string str)
{
    cout << "Invalid argument " << str << endl;
    exit(0);
}

//too many files output
void many_file_error()
{
    cout << "Too many file names" << endl;
    exit(0);
}

//bad file output
void bad_file_error(string file)
{
    cout << "Could not open " << file << endl;
    exit(0);
}

//skips a comment line
void skip_comment_line(istream *s)
{
    while(s -> peek() != '\n')
        s -> get();
    s -> get();
}

//output summary function
void output_sum(map<TokenTypes, int> &counter, int currentLine)
{
    int total_tokens = 0;
    map<TokenTypes, int> :: iterator it;
    for(it = counter.begin(); it != counter.end(); it++)
    {
        total_tokens += it -> second;
    }

    cout << "Total Lines: " << currentLine << endl;
    cout << "Total tokens: " << total_tokens << endl;
    if(total_tokens > 1)
    {
        vector<string> most_freq;
        make_most_freq_num (counter, most_freq);
        cout << "Most frequently used tokens: ";
        int vec_size = most_freq.size();
        int count = 0;
        for(vector<string> :: iterator it = most_freq.begin(); it != most_freq.end(); it++)
        {
            count++;
            cout << *it;
            if(count < vec_size)
                cout << ", ";
        }
        cout << endl;
      }

}

void output_v(map<TokenTypes, list<string>> &tokens, vector<string> &v_str)
{

    for(vector<string> :: iterator it = v_str.begin(); it != v_str.end(); it++)
    {
        cout << *it << endl;
    }

}


//stats output function
void output_stats(map<TokenTypes, int> counter, map<TokenTypes, list<string>> tokens, vector<string> id_vec)
{
    cout << "Total IDs: " << counter[TokenTypes::ID];
    if(counter[TokenTypes::ID] > 0)
    {
        cout << endl;
        cout << "List of IDs: ";
        sort(id_vec.begin(),id_vec.end());
        int vec_size = id_vec.size();
        int count = 0;
        for(vector<string> :: iterator it = id_vec.begin(); it != id_vec.end(); it++)
        {
            count++;
            cout << *it;
            if(count < vec_size)
                cout << ", ";
        }
    }
    cout << endl;
}

void output_ERR(int line, string lexeme)
{
    cout << "Error on line " << line + 1 << " (" << lexeme << ")" << endl;
}
//function that forms a vector for -sum output
void make_str_vec (vector<string> &v_str, string lex, TokenTypes token)
{
    if(token == TokenTypes :: STRING || token == TokenTypes :: ID || token == TokenTypes :: ICONST || token == TokenTypes :: FCONST)
    {
        string str = token_names[token] + " (" + lex + ")";
        v_str.push_back(str);
    }
    else
    {
        v_str.push_back(token_names[token]);
    }
}


//function makes vector for -stats output
int make_id_vec (vector<string> &id_vec, string lex)
{
    if(find(id_vec.begin(), id_vec.end(), lex) != id_vec.end())
        return 0;
    else
        id_vec.push_back(lex);

    return 0;
}

//function makes vector of most frequently used tokens
void make_most_freq_num (map<TokenTypes, int> counter, vector<string> &most_freq)
{
    int high_val[12] = {0};
    int i = 0;
    int value_count = 0;
    for(map<TokenTypes, int> :: iterator it = counter.begin(); it != counter.end(); it++)
    {
        if(value_count >= 1 && it -> second > high_val[0])
        {
            for(int x = 0; x < value_count; x++)
            {
                most_freq.pop_back();
                i--;
                value_count--;
            }
        }
        if(it -> second > high_val[0])
        {
            high_val[i] = it -> second;
            most_freq.push_back(::token_names[it -> first]);
            i++;
            value_count++;
        }
        else if(it -> second == high_val[0])
        {
            high_val[i] = it -> second;
            most_freq.push_back(::token_names[it -> first]);
            i++;
            value_count++;
        }
    }
}
