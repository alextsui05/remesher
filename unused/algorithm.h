#ifndef ALGORITHM_HEADER
#define ALGORITHM_HEADER

#include <string>

#include "defines.h"
#include "thread.h"

REMESHER_NAMESPACE_BEGIN

/* Base class for algorithms. Handles threading and common behaviour. */
class Algorithm : public Thread
{
  protected:
    Algorithm (void);
    virtual ~Algorithm (void);

    /* Algorithm implementation goes here. */
    virtual void* run (void) = 0;

    /* Algorithms set their status using this functions. */
    void set_running (bool running);
    void set_progress (float progress);
    void set_message (std::string const& message);

  public:
    /* Interface to start an algorithm. */
    void start_algorithm (bool in_thread = false);
};

/* ---------------------------------------------------------------- */

inline
Algorithm::Algorithm (void)
{
}

inline
Algorithm::~Algorithm (void)
{
}

inline void
Algorithm::start_algorithm (bool in_thread)
{
  if (in_thread)
    this->pt_create();
  else
    this->run();
}

REMESHER_NAMESPACE_END

#endif /* ALGORITHM_HEADER */
