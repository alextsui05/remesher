#ifndef ARGUMENTS_HEADER
#define ARGUMENTS_HEADER

#include <string>

class Arguments
{
  private:
    Arguments (void) {}

  public:
    static std::string load_model;
    static void show_help (void);
    static bool has_argument (int argc, char** argv, int idx);

  public:
    static void init (int argc, char** argv);
};

#endif /* ARGUMENTS_HEADER */
