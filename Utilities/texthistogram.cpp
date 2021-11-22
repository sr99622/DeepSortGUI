#include "texthistogram.h"

TextHistogram::TextHistogram(const QString& title, QSettings *settings, const QString& settingsKey)
{
    this->settings = settings;
    this->settingsKey = settingsKey;

    txtMaxValue = new NumberTextBox(settings, settingsKey + "/txtMaxValue", INTEGER_NUMBER, "max");
    txtMaxValue->setMaximumWidth(txtMaxValue->fontMetrics().boundingRect("00000").width());

    chkAutoMax = new CheckBox("auto", settings, settingsKey + "/chkAutoMax");
    connect(chkAutoMax, SIGNAL(clicked(bool)), this, SLOT(autoMaxChecked(bool)));
    if (chkAutoMax->isChecked())
        txtMaxValue->setEnabled(false);
    chkEnabled = new CheckBox("on", settings, settingsKey + "/chkEnabled");

    number_of_buckets = 10;
    lblBuckets = new QLabel();
    lblBuckets->setAlignment(Qt::AlignRight);
    lblHistogram = new QLabel();
    lblHeights = new QLabel();
    lblHeights->setAlignment(Qt::AlignRight);
    lblTitle = new QLabel(title);
    lblTitle->setAlignment(Qt::AlignCenter);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(lblTitle,           0, 0, 1, 6);
    layout->addWidget(lblBuckets,         1, 0, 1, 1);
    layout->addWidget(lblHistogram,       1, 1, 1, 4);
    layout->addWidget(lblHeights,         1, 3, 1, 1);
    layout->addWidget(txtMaxValue->lbl,   2, 0, 1, 1);
    layout->addWidget(txtMaxValue,        2, 1, 1, 1);
    layout->addWidget(chkAutoMax,         2, 2, 1, 2);
    layout->addWidget(chkEnabled,         2, 4, 1, 2);

    setLayout(layout);
}

void TextHistogram::histogram(const std::vector<int>& data)
{
    if (!chkEnabled->isChecked())
        return;

    if (data.size() == 0)
        return;

    //int number_of_buckets = 10;
    int scaled_histogram_height = 20;

    std::vector<int> buckets;
    for (int i = 0; i < number_of_buckets; i++)
        buckets.push_back(0);

    if (chkAutoMax->isChecked())
        max_data_value = data[distance(std::begin(data), max_element(std::begin(data), std::end(data)))];
    else
        max_data_value = std::min(txtMaxValue->intValue(), data[distance(std::begin(data), max_element(std::begin(data), std::end(data)))]);

    if (max_data_value == 0)
        return;

    for (size_t i = 0; i < data.size(); i++) {
        int index = (int)(data[i] * number_of_buckets / (float)max_data_value);
        if (index < number_of_buckets)
            buckets[index]++;
    }

    int max_bucket_value = buckets[distance(std::begin(buckets), max_element(std::begin(buckets), std::end(buckets)))];

    QString strBuckets;
    QString strHistogram;
    QString strHeights;
    int bucket_top = 0;
    for (int i = 0; i < number_of_buckets; i++) {
        bucket_top = (int)((i + 1) * max_data_value / (float)number_of_buckets);
        QTextStream(&strBuckets) << bucket_top << '\n';
        QTextStream(&strHeights) << buckets[i] << '\n';


        for (int j = 0; j < (int)(buckets[i] * scaled_histogram_height / (float)max_bucket_value); j++)
             QTextStream(&strHistogram) << "*";
         QTextStream(&strHistogram) << '\n';
    }

    lblBuckets->setText(strBuckets);
    int lblBucketsWidth = lblBuckets->fontMetrics().boundingRect(QString::number(bucket_top)).width() * 1.5;
    lblBuckets->setFixedWidth(lblBucketsWidth);
    lblHeights->setText(strHeights);
    int lblHeightsWidth = lblHeights->fontMetrics().boundingRect("00000").width() * 1.5;
    lblHeights->setFixedWidth(lblHeightsWidth);
    lblHistogram->setText(strHistogram);
    int lblHistogramWidth = lblHistogram->fontMetrics().boundingRect("*").width() * 25;
    lblHistogram->setFixedWidth(lblHistogramWidth);
    setFixedWidth(lblBucketsWidth + lblHeightsWidth + lblHistogramWidth);
}

void TextHistogram::autoMaxChecked(bool arg)
{
    txtMaxValue->setEnabled(!arg);
}
