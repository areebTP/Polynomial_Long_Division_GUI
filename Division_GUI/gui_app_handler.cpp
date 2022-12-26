#include <QDebug>
#include <QRegularExpression>
#include <regex>
#include "gui_app_handler.h"

// Initialising static member variable
QRegularExpression Gui_app_handler::m_regex1 = QRegularExpression("\\^-?\\d+");


Gui_app_handler::Gui_app_handler()
    :m_divisor{new Divisor()}
{}

void Gui_app_handler::parseInputData(QString &&repititions,
                                     QString &&numerator,
                                     QString &&denominator,
                                     QLabel* errorOutput)
{
    if (!repititions.size() || !numerator.size() || !denominator.size()){
        errorOutput->setText("Cannot compute result, empty fields detected.\n"
                             "Please click the 'HELP' button for more information.");
        errorOutput->setAlignment(Qt::AlignJustify);
        return;
    }

    QString _repititions = repititions.remove(' ');
    QString _numerator   = numerator  .remove(' ');
    QString _denominator = denominator.remove(' ');

    if (!verifyPolynomial(_numerator) || !verifyPolynomial(_denominator)){
        errorOutput->setText("Cannot compute result, either the numerator or denominator is entered incorrectly.\n"
                             "Please click the 'HELP' button for more information.");
        errorOutput->setAlignment(Qt::AlignJustify);
        return;
    }

    m_repititions = _repititions.toInt();

    int numPower = maxPower(_numerator);
    int denPower = maxPower(_denominator);

    // parse and store the input data into the member variables
    m_numerator   = parseInputData(_numerator);
    m_denominator = parseInputData(_denominator);

    if (numPower < denPower){
        int difference = denPower - numPower;
        for (int i{}; i< difference; ++i){
            m_numerator.prepend(0);
        }
    }
    else if(numPower > denPower){
        int difference = numPower - denPower;
        for (int i{}; i< difference; ++i){
            m_denominator.prepend(0);
        }
    }


    qDebug()<<"Parsed data:";
    printDebugger(m_numerator);
    printDebugger(m_denominator);

    m_divisor->setParameters(m_numerator, m_denominator, checkCoeff(_numerator), std::max(numPower, denPower));
    m_divisor->setRepititions(m_repititions);
    m_divisor->performDivision();
    qDebug() << m_divisor->getResult();
}

QList<double> Gui_app_handler::parseInputData(QString &data)
{
    QChar coeff = checkCoeff(data);
    data.remove(m_regex1);
    QList<QString> _splitter = data.split(coeff);
    QList<double> ret;

    // transform into double from QString
    for (int i{};i<_splitter.size();++i){
        if (_splitter[i] != ""){
            ret.push_back(_splitter[i].toDouble());
        }
    }
    return ret;
}

QChar Gui_app_handler::checkCoeff(const QString &num_den)
{
    int coeffIndex{};
    bool conversionSatus = true;
    QChar plus  = '+';
    QChar minus = '-';

    if (num_den[0] == plus || num_den[0] == minus){
        for (int i = 1;i<num_den.size();++i){
            QString(num_den[i]).toInt(&conversionSatus, 10);
            if (conversionSatus == false){ // the first non-number found
                coeffIndex = i;
                break;
            }
        }
    }
    else{
        for (int i{};i<num_den.size();++i){
            QString(num_den[i]).toInt(&conversionSatus, 10);
            if (conversionSatus == false){ // the first non-number found
                coeffIndex = i;
                break;
            }
        }
    }
    return num_den[coeffIndex];
}

int Gui_app_handler::maxPower(const QString &num_den){
    int startIndex{};
    int endIndex{};

    for (startIndex = 0;startIndex<num_den.size(); ++startIndex){
        if (num_den[startIndex] == '^'){
            if (num_den[startIndex+1] == '-'){
                for (endIndex =startIndex+2; num_den[endIndex] != '+'&& num_den[endIndex] != '-';
                    ++endIndex){}
                    startIndex++;
                    break; // break out of the loop
            }
            else if (num_den[startIndex+1] == '+'){
                for (endIndex =startIndex+2; num_den[endIndex] != '+'&& num_den[endIndex] != '-';
                    ++endIndex){}
                    startIndex++;
                    break; // break out of the loop
            }
            else{
                for (endIndex =startIndex+1; num_den[endIndex] != '+'&& num_den[endIndex] != '-';
                    ++endIndex){}
                    startIndex++;
                    break; // break out of the loop
            }
        }
    }
    std::string _num_den = num_den.toStdString();
    std::string number(_num_den.begin()+startIndex, _num_den.begin()+endIndex);
    return std::stoi(number);
}

bool Gui_app_handler::verifyPolynomial(const QString &polynomial){
    const std::regex _regex("((-|\\+)?[0-9]+[a-z]\\^(-|\\+)?[0-9]+)+");
    return regex_match(polynomial.toStdString(), _regex);
}

