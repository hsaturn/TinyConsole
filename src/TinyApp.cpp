#include "TinyApp.h"

TinyApp::TinyApp(TinyTerm* term) : term(term), state_(RUNNING)
{
  if (term)
  {
    cb = term->onKey([this](TinyTerm::KeyCode c) { this->onKey(c); });
    cbm = term->onMouse([this](const TinyTerm::MouseEvent& e) { this->onMouse(e); });
  }
}

TinyApp::~TinyApp()
{
  if (term)
  {
    term->onKey(cb);
    term->onMouse(cbm);
  }
}

void TinyApp::terminate()
{
  state_ = ENDED;
}