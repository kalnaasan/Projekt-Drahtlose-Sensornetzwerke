import {Component, EventEmitter, Input, OnInit, Output} from '@angular/core';
import {
  ApexAxisChartSeries,
  ApexChart,
  ApexDataLabels,
  ApexFill,
  ApexMarkers,
  ApexNonAxisChartSeries,
  ApexTitleSubtitle, ApexTooltip, ApexXAxis
} from "ng-apexcharts";
import {ApiService} from "../../services/api.service";
import {DatePipe} from "@angular/common";
import {ValueMeasure} from "../../model/value-measure";


@Component({
  selector: 'app-chart',
  templateUrl: './chart.component.html',
  styleUrls: ['./chart.component.scss']
})
export class ChartComponent implements OnInit {
  @Input() nameChart: any;
  @Input() srcData: any;
  @Input() resolveStatus!: boolean;
  @Output() message: EventEmitter<{ sensorName: string, status: string, value: number }> = new EventEmitter<{
    sensorName: string,
    status: string,
    value: number
  }>();
  public data: any[] = [];
  public averageValues: number = 0;
  public activeOptionButton = 'now';
  public minValue = 0;
  public minNormalValue = 0
  public maxNormalValue = 0
  public maxValue = 0;
  series: ApexAxisChartSeries | ApexNonAxisChartSeries = [];
  title: ApexTitleSubtitle = {};
  chart: ApexChart = {
    type: 'area',
    height: 350
  };
  dataLabels: ApexDataLabels = {
    enabled: false
  };
  markers: ApexMarkers = {
    size: 0
  };
  xaxis: ApexXAxis = {
    type: 'datetime',
    min: undefined,
    max: undefined,
    tickAmount: 6
  };
  tooltip: ApexTooltip = {
    x: {
      format: 'dd MMM yyyy - hh:mm:ss',
    }
  };
  fill: ApexFill = {
    type: 'gradient',
    gradient: {
      shadeIntensity: 1,
      opacityFrom: 0.7,
      opacityTo: 0.9,
      stops: [0, 100]
    }
  };

  public updateOptionsData: { [key: string]: ApexXAxis } = {
    'week': {
      type: 'datetime',
      min: new Date().getTime() - 604800000,
      max: new Date().setHours(23, 59, 59),
      tickAmount: 6
    },
    'today': {
      type: 'datetime',
      min: new Date().setHours(0, 0, 1),
      max: new Date().setHours(23, 59, 59),
      tickAmount: 6
    },
    'now': {
      type: 'datetime',
      min: new Date().getTime() - 3600000,
      max: new Date().getTime() + 1800000,
      tickAmount: 6
    }
  };

  constructor(private apiService: ApiService,
              private datePipe: DatePipe) {
  }

  ngOnInit(): void {
    const from = this.timestampToDate(this.updateOptionsData[this.activeOptionButton].min || Date.now());
    console.log(from);
    const to = this.timestampToDate(this.updateOptionsData[this.activeOptionButton].max || Date.now());
    console.log(to);
    this.getData(from, to);
    // console.log('Chart: ', this.nameChart, this.resolveStatus);

  }

  timestampToDate(timestamp: number): string | undefined {
    const date = new Date(timestamp);
    return this.datePipe.transform(date, 'yyyy-MM-ddTHH:mm:ss')?.toString();
  }

  addNewItem(sensorName: string, status: string, value: number) {
    this.message.emit({sensorName: sensorName, status: status, value: value});
  }

  public updateOptions(option: any): void {
    this.activeOptionButton = option;
    this.xaxis = this.updateOptionsData[option];
    if (this.activeOptionButton === 'week') {
      const from = this.convertNumberToDate(this.updateOptionsData[this.activeOptionButton].min);
      const to = this.convertNumberToDate(this.updateOptionsData[this.activeOptionButton].max);
      this.getData(from, to);
    }
  }

  private convertDataToChartData(src: ValueMeasure[]) {
    if (this.nameChart.includes('temp') || this.nameChart.includes('hum')) {
      return src.map((valueMeasure: ValueMeasure) => [Date.parse(valueMeasure.readAt), Math.round(valueMeasure.value / 1000)]);
    } else if (this.nameChart.includes('voc')) {
      return src.map((valueMeasure: ValueMeasure) => [Date.parse(valueMeasure.readAt), Math.round(valueMeasure.value / 10)]);
    }
    return src.map((valueMeasure: ValueMeasure) => [Date.parse(valueMeasure.readAt), valueMeasure.value]);
  }

  private convertNumberToDate(value: any) {
    let d = new Date(value),
      month = '' + (d.getMonth() + 1),
      day = '' + d.getDate(),
      year = d.getFullYear();

    month = (month.length < 2) ? '0' + month : month;
    day = (day.length < 2) ? '0' + day : day;

    return [year, month, day].join('-');
  }

  private getData(from: string | undefined, to: string | undefined) {
    if (this.nameChart !== null) {
      this.apiService.getValueBySensorTypeAndDate(this.nameChart, from, to).subscribe({
        next: (res: any) => {
          this.data = this.convertDataToChartData(res.data);
          this.xaxis = this.updateOptionsData[this.activeOptionButton];
          this.series = [{
            name: this.nameChart,
            data: this.data
          }];
          this.averageValues = Math.round(this.calculateAverage());
          if (this.nameChart.toLowerCase().includes("temp")) {
            // 16 - 18 - 24 - 26
            this.setLimitValue(16, 18, 24, 26);
          } else if (this.nameChart.toLowerCase().includes('hum')) {
            // 30, 40, 60, 70
            this.setLimitValue(30, 40, 60, 70);
          } else if (this.nameChart.toLowerCase().includes('voc')) {
            // 50 - 51 - 100
            this.setLimitValue(0, 0, 50, 100);
          } else if (this.nameChart.toLowerCase().includes('co2')) {
            // 1000 - 1001 - 2000 -
            this.setLimitValue(0, 0, 1000, 2000);
          }

          this.title = {
            text: this.nameChart
          };
          this.calculateStatus();
        },
        error: (err: any) => console.log(err),
      });

    }

  }

  public calculateAverage(): number {
    /*let sum = 0;
    for (let i = 0; i < this.data.length; i++) {
      sum += this.data[i][1];
    }
    return sum / this.data.length;*/
    return this.data[this.data.length - 1][1]
  }

  private setLimitValue(min: number, minNormal: number, maxNormal: number, max: number) {
    this.minValue = min;
    this.minNormalValue = minNormal;
    this.maxNormalValue = maxNormal;
    this.maxValue = max;
  }

  private calculateStatus() {
    if (this.averageValues < this.minValue || this.averageValues > this.maxValue) {
      this.addNewItem(this.nameChart, 'danger', this.averageValues);
    } else if ((this.averageValues >= this.minValue && this.averageValues < this.minNormalValue) ||
      (this.averageValues > this.maxNormalValue && this.averageValues < this.maxValue)) {
      this.addNewItem(this.nameChart, 'success', this.averageValues);
    } else if (this.averageValues >= this.minNormalValue && this.averageValues <= this.maxNormalValue) {
      this.addNewItem(this.nameChart, 'warning', this.averageValues);
    }
  }
}
