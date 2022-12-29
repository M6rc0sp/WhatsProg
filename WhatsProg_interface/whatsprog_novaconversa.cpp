#include <string>
#include <QMessageBox>
#include "whatsprog_novaconversa.h"
#include "ui_whatsprog_novaconversa.h"
///Autores: Marcos Paulo Barbosa && Luisa de Moura Galvao Mathias
WhatsProgNovaConversa::WhatsProgNovaConversa(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::WhatsProgNovaConversa)
{
  ui->setupUi(this);
}

WhatsProgNovaConversa::~WhatsProgNovaConversa()
{
  delete ui;
}

void WhatsProgNovaConversa::on_buttonBox_accepted()
{
  // Falta fazer
}
