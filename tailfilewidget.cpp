#include "tailfilewidget.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFontDatabase>
#include <QScrollBar>
#include <QLineEdit>

#include "highlightedtextedit.h"

TailFileWidget::TailFileWidget(const QString& filePath, QWidget* parent)
    : QDockWidget(parent), fixRegex(QRegularExpression("8=FIX.*\x01")) {

    textEdit = new HighlightedTextEdit(this);

    fixTable = new QTableWidget(this);

    // Carregar a fonte personalizada
    int fontId = QFontDatabase::addApplicationFont("./assets/fonts/CaskaydiaMonoNerdFont-Light.ttf");
    if (fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont customFont(fontFamily);
        customFont.setPointSize(10); // Defina o tamanho da fonte conforme necessário

        // Definir a fonte como padrão para o QTextEdit e outros componentes
        textEdit->setFont(customFont);

        // Se tiver outros componentes, aplique a fonte neles também (exemplo: fixTable)
        fixTable->setFont(customFont);
    } else {
        qDebug() << "Falha ao carregar a fonte personalizada.";
    }

    setWidget(textEdit);

    // Inicializa o FileMonitor e conecta o sinal de nova linha
    fileMonitor = new FileMonitor(filePath, this);
    connect(fileMonitor, &FileMonitor::newLineRead, this, &TailFileWidget::onNewLine);

    // Conecta a mudança de posição do cursor para verificar se a linha contém um registro FIX
    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &TailFileWidget::onCursorPositionChanged);

    // Setup da tabela de FIX, inicialmente escondida
    setupFixTable();
    fixTable->setVisible(false);

    fileMonitor->Start();
}

void TailFileWidget::addHighlighter(Highlighter* highlighter) {
    highlighters.append(highlighter);
}

void TailFileWidget::onNewLine(qint64 pos, const QString& line) {
    // Verificar se a última linha está visível (ou seja, se a barra de rolagem está no final)
    QScrollBar* scrollBar = textEdit->verticalScrollBar();
    bool scrollAtBottom = scrollBar != nullptr && scrollBar->value() == scrollBar->maximum();

    // Adicionar a nova linha no QTextEdit
    textEdit->append(line);

    // Se a barra de rolagem estava no final, rolar novamente para o final
    if (scrollAtBottom) {
        scrollBar->setValue(scrollBar->maximum());
    }

    // Aplica os highlights ativos
    for (Highlighter* highlighter : highlighters) {
        highlighter->handleNewLine(pos, line);
    }
}


void TailFileWidget::onCursorPositionChanged() {
    // Obtém o conteúdo da linha onde o cursor está
    QString currentLine = getCurrentLine();

    // Verifica se a linha corresponde ao padrão de registro FIX
    QRegularExpressionMatch match = fixRegex.match(currentLine);
    if (match.hasMatch()) {
        processFixLine(match.captured());
        fixTable->setVisible(true); // Mostra a tabela
    } else {
        fixTable->setVisible(false); // Esconde a tabela
    }
}

QString TailFileWidget::getCurrentLine() {
    // Obtém o cursor atual e o bloco de texto (linha)
    QTextCursor cursor = textEdit->textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    return cursor.selectedText();
}

void TailFileWidget::processFixLine(const QString& line) {
    // Exemplo simples de processamento de uma linha do protocolo FIX
    QStringList fields = line.split("\x01");
    fixTable->setRowCount(fields.size());

    int row = 1;
    for (const QString& field : fields) {
        QStringList tagValue = field.split("=");
        if (tagValue.size() == 2) {
            QString tag = tagValue[0];
            QString value = tagValue[1];

            // Preencher a tabela (você pode adicionar a lógica para preencher Tag Name e Domain Description)
            fixTable->setItem(row, 0, new QTableWidgetItem(tag));
            fixTable->setItem(row, 1, new QTableWidgetItem(value));
            fixTable->setItem(row, 2, new QTableWidgetItem("Tag Name")); // Exemplo
            fixTable->setItem(row, 3, new QTableWidgetItem("Domain Description")); // Exemplo
            row++;
        }
    }
}

void TailFileWidget::setupFixTable() {
    fixTable->setColumnCount(4);
    fixTable->setHorizontalHeaderLabels({"Tag", "Value", "Tag Name", "Domain Description"});
    fixTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    fixTable->setSelectionMode(QAbstractItemView::NoSelection);
    fixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fixTable->setSortingEnabled(true); // Habilitar ordenação

    // Adicionar linha de filtro
    addFilterRow();

    // Layout para colocar o QTextEdit e o QTableWidget lado a lado
    QWidget* container = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->addWidget(textEdit);
    layout->addWidget(fixTable);
    container->setLayout(layout);

    setWidget(container);
}

void TailFileWidget::addFilterRow() {
    int columnCount = fixTable->columnCount();
    fixTable->insertRow(0);

    for (int col = 0; col < columnCount; ++col) {
        QLineEdit* filterEdit = new QLineEdit(this);
        filterEdit->setPlaceholderText("Filter...");
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

