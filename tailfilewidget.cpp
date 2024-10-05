#include "tailfilewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFontDatabase>
#include <QScrollBar>
#include <QLineEdit>
#include <QSplitter>
#include <QTimer>
#include <QApplication>

#include "LogListView.hpp"
#include "TailFileTitleBar.hpp"
#include "HighlightRuleManager.hpp"

TailFileWidget::TailFileWidget(const QString& filePath, FixDictionary *fixDictionary, QWidget* parent)
    : QDockWidget(parent), fixRegex(QRegularExpression("8=FIX.*\u2401")) {

    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    QFileInfo fileInfo(filePath);
    setWindowTitle(fileInfo.fileName());
    setTitleBarWidget(new TailFileTitleBar(this, filePath, fileInfo.fileName(), this));

    // Definir para deletar o widget ao fechar a janela
    setAttribute(Qt::WA_DeleteOnClose);

    // Default Highlights
    // filter
    m_highlightRules.push_back({QRegularExpression(""), true, Qt::white, true, QColor::fromRgbF(0, 0.5, 0)});
    // control chars
    m_highlightRules.push_back({QRegularExpression("[\u2400-\u240F]"), true, Qt::white, true, QColor::fromRgbF(0.5, 0, 0)});

    m_fixDictionary = fixDictionary;

    fileModel = new LogFileModel(filePath, this);
    listView = new LogListView(fileModel, &m_highlightRules, this);
    fixTable = new QTableWidget(this);

    // Carregar a fonte personalizada
    int fontId = QFontDatabase::addApplicationFont("./assets/fonts/CaskaydiaMonoNerdFont-Light.ttf");
    if (fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont customFont(fontFamily);
        customFont.setPointSize(10); // Defina o tamanho da fonte conforme necessário

        // Definir a fonte como padrão para o QTextEdit e outros componentes
        listView->setFont(customFont);

        // Se tiver outros componentes, aplique a fonte neles também (exemplo: fixTable)
        fixTable->setFont(customFont);
    } else {
        qDebug() << "Falha ao carregar a fonte personalizada.";
    }

    // Cria os proxies de filtros do log
    containsProxyModel = new ContainsFilterProxyModel(this);
    regexProxyModel = new RegExFilterProxyModel(this);
    containsProxyModel->setSourceModel(fileModel);
    regexProxyModel->setSourceModel(fileModel);

    listView->setModel(containsProxyModel);

    // Conecta a mudança de posição do cursor para verificar se a linha contém um registro FIX
    connect(listView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &current, const QModelIndex &previous) {
        if (current.isValid()) {
            QString selectedLine = listView->model()->data(current, Qt::DisplayRole).toString();
            // Verifica se a linha corresponde ao padrão de registro FIX
            QRegularExpressionMatch match = fixRegex.match(selectedLine);
            if (match.hasMatch()) {
                processFixLine(match.captured());
                fixTable->setVisible(true); // Mostra a tabela
                //QMetaObject::invokeMethod(fixTable, "resizeColumnsToContents", Qt::QueuedConnection);
            } else {
                fixTable->setVisible(false); // Esconde a tabela
            }
        }
    });

    // Layout principal para o ListView e o campo de filtro
    QVBoxLayout* mainLayout = new QVBoxLayout;

    // Layout para o filtro
    QHBoxLayout* filterLayout = new QHBoxLayout;

    bool fileTooLarge = fileModel->rowCount() > 500000;

    // Adicionar o QLineEdit para o filtro
    filterEdit = new QLineEdit(this);
    filterEdit->setPlaceholderText(fileTooLarge ? tr("Too large to filter...") : tr("Filter..."));
    filterEdit->setEnabled(!fileTooLarge);

    filterLayout->addWidget(filterEdit);

    // Adicionar o checkbox para Regex
    regexCheckBox = new QCheckBox(tr("Use RegEx"), this);
    regexCheckBox->setEnabled(!fileTooLarge);

    filterLayout->addWidget(regexCheckBox);

    QPushButton* highlights = new QPushButton(tr("Highlights"), this);

    filterLayout->addWidget(highlights);

    connect(highlights, &QPushButton::clicked, this, [this, fileInfo]() {
        HighlightRuleManager highlightRuleManager(&m_highlightRules, fileInfo.fileName(), nullptr);
        highlightRuleManager.exec();
    });

    QTimer* filterDebounce = new QTimer(this);
    filterDebounce->setSingleShot(true);

    connect(filterEdit, &QLineEdit::textChanged, this, [this, filterDebounce]() {
        filterDebounce->start(300);
    });
    connect(filterDebounce, &QTimer::timeout, this, [this]() {
        applyFilter(filterEdit->text());
    });

    connect(regexCheckBox, &QCheckBox::stateChanged, this, [this]() {
        if (regexCheckBox->isChecked()) {
            regexProxyModel->setFilter(filterEdit->text());
            listView->setModel(regexProxyModel);
        } else {
            containsProxyModel->setFilter(filterEdit->text());
            listView->setModel(containsProxyModel);
        }
    });

    // Adicionar o layout de filtro ao layout principal
    mainLayout->addLayout(filterLayout);

    // Adicionar o ListView ao layout principal
    mainLayout->addWidget(listView);

    logWidget = new QWidget(this);
    logWidget->setLayout(mainLayout);

    // Setup da tabela de FIX, inicialmente escondida
    setupFixTable();
    fixTable->setVisible(false);
}

void TailFileWidget::applyFilter(const QString& filterText) {
    if(regexCheckBox->isChecked()) {
        regexProxyModel->setFilter(filterText);
        m_highlightRules[1].regex.setPattern(filterText);
    } else {
        containsProxyModel->setFilter(filterText);
        m_highlightRules[1].regex.setPattern(QRegularExpression::escape(filterText));
    }
    QMetaObject::invokeMethod(listView, "repaint", Qt::QueuedConnection);
}


void TailFileWidget::processFixLine(const QString& line) {
    // Exemplo simples de processamento de uma linha do protocolo FIX
    QStringList fields = line.split("\u2401");
    fixTable->setRowCount(fields.size());

    int row = 1;
    for (const QString& field : fields) {
        QStringList tagValue = field.split("=");
        if (tagValue.size() == 2) {
            QString tag = tagValue[0];
            QString value = tagValue[1];
            QString tagName;
            QString valueDescription;

            m_fixDictionary->TryGetTagNameAndFieldValueDescription(tag.toInt(), value, tagName, valueDescription);

            // Preencher a tabela (você pode adicionar a lógica para preencher Tag Name e Domain Description)
            fixTable->setItem(row, 0, new QTableWidgetItem(tag));
            fixTable->setItem(row, 1, new QTableWidgetItem(value));
            fixTable->setItem(row, 2, new QTableWidgetItem(tagName));
            fixTable->setItem(row, 3, new QTableWidgetItem(valueDescription));
            row++;
        }
    }
}

void TailFileWidget::setupFixTable() {
    fixTable->setColumnCount(4);
    fixTable->setHorizontalHeaderLabels({tr("Tag"), tr("Value"), tr("Tag Name"), tr("Domain Description")});
    fixTable->horizontalHeader()->setMinimumSectionSize(5);
    fixTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    fixTable->setSelectionMode(QAbstractItemView::NoSelection);
    fixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fixTable->setSortingEnabled(false); // nao Habilitar ordenação

    // Adicionar linha de filtro
    addFilterRow();

    // Layout para colocar o QTextEdit e o QTableWidget lado a lado
    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);

    splitter->addWidget(logWidget);
    splitter->addWidget(fixTable);

    setWidget(splitter);
}

void TailFileWidget::addFilterRow() {
    int columnCount = fixTable->columnCount();
    fixTable->insertRow(0);

    for (int col = 0; col < columnCount; ++col) {
        QLineEdit* filterEdit = new QLineEdit(this);
        filterEdit->setPlaceholderText(tr("Filter..."));
        connect(filterEdit, &QLineEdit::textChanged, this, [this, col](const QString& text) {
            filterTable(col, text); // Função de filtro para aplicar na coluna
        });
        fixTable->setCellWidget(0, col, filterEdit);
    }
}

void TailFileWidget::filterTable(int column, const QString& filterText) {
    QRegularExpression regex(filterText, QRegularExpression::CaseInsensitiveOption);

    for (int row = 1; row < fixTable->rowCount(); ++row) { // Ignora a linha de filtro
        bool match = fixTable->item(row, column)->text().contains(regex);
        fixTable->setRowHidden(row, !match);
    }
}

