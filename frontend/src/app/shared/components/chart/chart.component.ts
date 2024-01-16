import {Component, Input, OnInit} from '@angular/core';
import {
  ApexAnnotations,
  ApexAxisChartSeries,
  ApexChart,
  ApexDataLabels,
  ApexFill,
  ApexMarkers,
  ApexNonAxisChartSeries,
  ApexTitleSubtitle,
  ApexTooltip,
  ApexXAxis
} from 'ng-apexcharts';
import {ValueMeasure} from "../../model/value-measure";
import {ApiService} from "../../services/api.service";

@Component({
  selector: 'app-chart',
  templateUrl: './chart.component.html',
  styleUrls: ['./chart.component.scss']
})
export class ChartComponent implements OnInit {
  @Input() nameChart: any;
  @Input() srcData: any;
  public data: any[]=[];
  public averageValues: number = 0;
  public activeOptionButton = 'today';
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
  annotations: ApexAnnotations = {
    yaxis: [
      {
        y: 30,
        borderColor: '#999',
        label: {
          text: 'Support',
          style: {
            color: '#fff',
            background: '#00E396'
          }
        }
      }
    ],
    xaxis: [
      {
        x: new Date('14 Nov 2012').getTime(),
        borderColor: '#999',
        label: {
          text: 'Rally',
          style: {
            color: '#fff',
            background: '#775DD0'
          }
        }
      }
    ]
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
      min: new Date().setHours(0, 0, 0),
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
  activeStatusOption: string = 'active';

  constructor(private apiService: ApiService) {
  }

  ngOnInit(): void {
    const from = this.convertNumberToDate(this.updateOptionsData[this.activeOptionButton].min);
    const to = this.convertNumberToDate(this.updateOptionsData[this.activeOptionButton].max);
    this.getData(from, to);
  }

  public updateOptions(option: any): void {
    this.activeOptionButton = option;
    this.xaxis = this.updateOptionsData[option];
    if (this.activeOptionButton=== 'week'){
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

  private getData(from: string, to: string){
    if (this.nameChart !== null){
      this.apiService.getValueBySensorTypeAndDate(this.nameChart, from, to).subscribe({
        next: (res: any) => {
          this.data = this.convertDataToChartData(res.data);
          this.xaxis = this.updateOptionsData[this.activeOptionButton];
          this.series = [{
            name: this.nameChart,
            data: this.data
          }];
          this.averageValues = Math.round(this.calculateAverage());
          if (this.nameChart.includes("temp")) {
            // 16 - 18 - 24 - 26
            this.minValue = 16;
            this.minNormalValue = 18
            this.maxNormalValue = 24
            this.maxValue = 26;
          } else if (this.nameChart.includes('hum')) {
            this.minValue = 30;
            this.minNormalValue = 40
            this.maxNormalValue = 60
            this.maxValue = 70;
          } else if (this.nameChart.includes('voc')) {
            // 50 - 51 - 100
            this.minValue = 0;
            this.minNormalValue = 0
            this.maxNormalValue = 50
            this.maxValue = 100;
          } else if (this.nameChart.includes('co2')) {
            // 1000 - 1001 - 2000 -
            this.minValue = 0;
            this.minNormalValue = 0
            this.maxNormalValue = 1000
            this.maxValue = 2000;
          }
          this.title = {
            text: this.nameChart
          };
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
}
