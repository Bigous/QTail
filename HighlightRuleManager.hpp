#ifndef HIGHLIGHTRULEMANAGER_HPP
#define HIGHLIGHTRULEMANAGER_HPP

#include <QApplication>
#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QMessageBox>

#include "HighlightRuleForm.hpp"

class HighlightRuleManager : public QDialog {
    Q_OBJECT
public:
    HighlightRuleManager(QList<HighlightRule> *ruleList, QString fileName, QWidget *parent = nullptr) : QDialog(parent), m_ruleList(ruleList) {
        setWindowTitle(tr("Highlight Manager - %1").arg(fileName));
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Tabela para listar as regras
        rulesTable = new QTableWidget(this);
        rulesTable->setColumnCount(3);  // Regex, Cores, Botões de Ação
        rulesTable->setHorizontalHeaderLabels({tr("Regex"), tr("Colors"), tr("Commands")});
        rulesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        mainLayout->addWidget(rulesTable);

        // Botão para adicionar nova regra
        QPushButton *addButton = new QPushButton(tr("Add new rule"), this);
        mainLayout->addWidget(addButton);
        connect(addButton, &QPushButton::clicked, this, &HighlightRuleManager::addNewRule);

        populateTable();

        adjustSize();
    }

private slots:
    void addNewRule() {
        HighlightRuleForm form;
        if (form.exec() == QDialog::Accepted) {
            HighlightRule newRule = form.getHighlightRule();
            m_ruleList->append(newRule);
            populateTable(); // Atualiza a tabela
        }
    }

    void editRule(int row) {
        HighlightRule &rule = (*m_ruleList)[row];

        HighlightRuleForm form(this);
        form.setRule(rule);  // Preenche o formulário com a regra selecionada
        if(row < 2)
            form.disableRegexEdit();  // Desabilita a edição do regex para as duas primeiras regras

        if (form.exec() == QDialog::Accepted) {
            rule = form.getHighlightRule();  // Atualiza a regra com as mudanças feitas
            populateTable();
        }
    }

    void deleteRule(int row) {
        if (row >= 2) {  // Não permitir excluir as duas primeiras regras
            if (QMessageBox::question(this, tr("Confirm deleting"), tr("Do You really wanna delete this rule?")) == QMessageBox::Yes) {
                m_ruleList->removeAt(row);
                populateTable();
            }
        }
    }

private:
    QTableWidget *rulesTable;
    QList<HighlightRule> *m_ruleList;

    void populateTable() {
        rulesTable->clearContents();
        rulesTable->setRowCount(m_ruleList->size());

        for (int i = 0; i < m_ruleList->size(); ++i) {
            const HighlightRule &rule = (*m_ruleList)[i];

            // Coluna 0: Regex
            QTableWidgetItem *regexItem = new QTableWidgetItem(rule.regex.pattern());
            regexItem->setFlags(regexItem->flags() & ~Qt::ItemIsEditable);  // Não editável
            rulesTable->setItem(i, 0, regexItem);

            // Coluna 1: Cores (checkboxes e quadradinhos de cor)
            QWidget *colorWidget = new QWidget(this);
            QHBoxLayout *colorLayout = new QHBoxLayout(colorWidget);
            colorLayout->setContentsMargins(0, 0, 0, 0);

            // Checkbox e quadrado de cor para o foreground
            QCheckBox *foregroundCheckBox = new QCheckBox(this);
            foregroundCheckBox->setChecked(rule.useForegroundColor);
            foregroundCheckBox->setEnabled(false);
            QLabel *foregroundColorLabel = new QLabel(this);
            foregroundColorLabel->setFixedSize(20, 20);
            foregroundColorLabel->setAutoFillBackground(true);
            foregroundColorLabel->setStyleSheet(QString("border: 1px solid black; background-color: %1").arg(rule.foregroundColor.name()));

            colorLayout->addWidget(foregroundCheckBox);
            colorLayout->addWidget(foregroundColorLabel);

            // Checkbox e quadrado de cor para o background
            QCheckBox *backgroundCheckBox = new QCheckBox(this);
            backgroundCheckBox->setChecked(rule.useBackgroundColor);
            backgroundCheckBox->setEnabled(false);
            QLabel *backgroundColorLabel = new QLabel(this);
            backgroundColorLabel->setFixedSize(20, 20);
            backgroundColorLabel->setAutoFillBackground(true);
            backgroundColorLabel->setStyleSheet(QString("border: 1px solid black; background-color: %1").arg(rule.backgroundColor.name()));

            colorLayout->addWidget(backgroundCheckBox);
            colorLayout->addWidget(backgroundColorLabel);

            colorWidget->setLayout(colorLayout);
            rulesTable->setCellWidget(i, 1, colorWidget);

            // Coluna 2: Botões de ação
            QWidget *actionWidget = new QWidget(this);
            QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
            actionLayout->setContentsMargins(0, 0, 0, 0);

            QPushButton *editButton = new QPushButton(actionWidget);
            editButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste));
            //editButton->setText("✏️");
            connect(editButton, &QPushButton::clicked, this, [=, this]() { editRule(i); });
            actionLayout->addWidget(editButton);

            if(i >= 2) {  // Não permitir excluir as duas primeiras regras
                QPushButton *deleteButton = new QPushButton(actionWidget);
                deleteButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete));
                //deleteButton->setText("❌");
                connect(deleteButton, &QPushButton::clicked, this, [=, this]() { deleteRule(i); });
                actionLayout->addWidget(deleteButton);
            }

            actionWidget->setLayout(actionLayout);
            rulesTable->setCellWidget(i, 2, actionWidget);

        }

        // reajusta o tamanho das colunas
        QMetaObject::invokeMethod(rulesTable, "resizeColumnsToContents", Qt::QueuedConnection);
    }
};

#endif // HIGHLIGHTRULEMANAGER_HPP
