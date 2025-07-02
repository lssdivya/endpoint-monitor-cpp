#include "monitorwindow.h"
#include <windows.h>
#include <tlhelp32.h>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <psapi.h>

// Constructor
MonitorWindow::MonitorWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Endpoint Process Monitor");
    resize(700, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Search Box
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Search for a process...");
    connect(searchBox, &QLineEdit::textChanged, this, &MonitorWindow::filterProcessList);

    // Process Table
    processTable = new QTableWidget(this);
    processTable->setColumnCount(2);
    processTable->setHorizontalHeaderLabels({"PID", "Process Name"});
    processTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    processTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // CPU & Memory Labels
    cpuUsageLabel = new QLabel("CPU Usage: Loading...", this);
    memoryUsageLabel = new QLabel("Memory Usage: Loading...", this);

    // Refresh Button
    refreshButton = new QPushButton("Refresh", this);
    connect(refreshButton, &QPushButton::clicked, this, &MonitorWindow::updateProcessList);

    // Terminate Button
    terminateButton = new QPushButton("Terminate Process", this);
    connect(terminateButton, &QPushButton::clicked, this, &MonitorWindow::terminateSelectedProcess);

    // Add widgets to layout
    layout->addWidget(searchBox);
    layout->addWidget(processTable);
    layout->addWidget(cpuUsageLabel);
    layout->addWidget(memoryUsageLabel);
    layout->addWidget(refreshButton);
    layout->addWidget(terminateButton);

    // Auto-update system resources every 2 seconds
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MonitorWindow::updateResourceUsage);
    updateTimer->start(2000);  // Update every 2 seconds

    updateProcessList();
}

// Function to update process list
void MonitorWindow::updateProcessList() {
    processTable->setRowCount(0);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    int row = 0;

    if (Process32First(snapshot, &processEntry)) {
        do {
            processTable->insertRow(row);
            processTable->setItem(row, 0, new QTableWidgetItem(QString::number(processEntry.th32ProcessID)));
            processTable->setItem(row, 1, new QTableWidgetItem(QString::fromWCharArray(processEntry.szExeFile)));

            // Highlight suspicious processes
            if (isSuspiciousProcess(QString::fromWCharArray(processEntry.szExeFile))) {
                for (int col = 0; col < 2; col++) {
                    processTable->item(row, col)->setBackground(Qt::red);
                }
            }

            row++;
        } while (Process32Next(snapshot, &processEntry));
    }
    CloseHandle(snapshot);
}

// Function to update CPU & Memory usage
void MonitorWindow::updateResourceUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    // Simulated CPU usage (replace with actual monitoring if needed)
    cpuUsageLabel->setText("CPU Usage: " + QString::number(rand() % 50 + 10) + "%");
    memoryUsageLabel->setText("Memory Usage: " + QString::number(memInfo.dwMemoryLoad) + "%");
}

// Function to terminate selected process
void MonitorWindow::terminateSelectedProcess() {
    QList<QTableWidgetItem *> selectedItems = processTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No process selected!");
        return;
    }

    int pid = selectedItems[0]->text().toInt();
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == nullptr) {
        QMessageBox::warning(this, "Error", "Failed to open process.");
        return;
    }

    if (TerminateProcess(hProcess, 0)) {
        QMessageBox::information(this, "Success", "Process terminated successfully.");
        updateProcessList();
    } else {
        QMessageBox::warning(this, "Error", "Failed to terminate process.");
    }

    CloseHandle(hProcess);
}

// Function to filter process list based on search input
void MonitorWindow::filterProcessList() {
    QString filterText = searchBox->text().toLower();
    for (int row = 0; row < processTable->rowCount(); ++row) {
        bool match = processTable->item(row, 1)->text().toLower().contains(filterText);
        processTable->setRowHidden(row, !match);
    }
}

// Function to check if a process is suspicious
bool MonitorWindow::isSuspiciousProcess(const QString &processName) {
    QStringList knownSuspicious = {"malware.exe", "trojan.exe", "keylogger.exe", "ransomware.exe"};
    return knownSuspicious.contains(processName.toLower());
}
