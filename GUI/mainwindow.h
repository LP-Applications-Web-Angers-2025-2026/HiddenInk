#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Fichier
    void ouvrirImage();
    void quitter();

    // Stéganographie – Cachage
    void cacherTexte();
    void cacherImage();

    // Stéganographie – Extraction
    void extraireTexte();
    void extraireImage();

    // Analyse
    void comparerImages();
    void genererHistogramme();
    void detecterStegano();

    // Utilitaires
    void genererCle();
    void effacerJournal();
    void afficherAide();

private:
    Ui::MainWindow *ui;
    QString derniereImageChargee;
    QString cleCouranteHex; // clé hex affichée
    void logMessage(const QString &msg);
    QString choisirFichier(const QString &titre, const QString &filtre);
    QString sauverFichier(const QString &titre, const QString &filtre);
    bool confirmer(const QString &texte, const QString &titre = "Confirmation");
    void appliquerStyle();
    void initialiserMenus();
    void initialiserOnglets();
    // Helpers cachage image
    QString preparerImageSecreteRedimensionnee(const QString &secretPath, const QString &carrierPath, int bits, QString &info);
    int calculerCapaciteMax(const QSize &carrierSize, int bits);
    int tailleImagePixels(const QSize &secretSize);
};

#endif // MAINWINDOW_H
