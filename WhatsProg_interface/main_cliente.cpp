#include "mysocket.h"
#include "whatsprog_main.h"
#include <QApplication>
///Autores: Marcos Paulo Barbosa && Luisa de Moura Galvao Mathias
WhatsProgDadosCliente DC;

tcp_mysocket sock;

std::thread thr;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  WhatsProgMain w;

  w.show();
  int result = a.exec();

  return result;
}
