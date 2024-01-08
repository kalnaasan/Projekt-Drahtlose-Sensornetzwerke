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
  data: any;
  public activeOptionButton = 'today';
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
    'today': {
      type: 'datetime',
      min: new Date('2024-01-08T00:00:00').getTime(),
      max: new Date('2024-01-08T23:59:59').getTime(),
      tickAmount: 6
    }
  };

  constructor(private apiService: ApiService) {
  }

  ngOnInit(): void {
    const from = this.convertNumberToDate(this.updateOptionsData[this.activeOptionButton].min);
    const to = this.convertNumberToDate(this.updateOptionsData[this.activeOptionButton].max);
    this.apiService.getValueBySensorTypeAndDate(this.nameChart, from, to).subscribe({
      next: (res: any) => {
        this.data = this.convertDataToChartData(res.data);
        this.xaxis = this.updateOptionsData[this.activeOptionButton];
        this.series = [{
          name: this.nameChart,
          data: this.data
        }];

        this.title = {
          text: this.nameChart
        };
      },
      error: (err: any) => console.log(err),
    });
  }

  public updateOptions(option: any): void {
    this.activeOptionButton = option;
    this.xaxis = this.updateOptionsData[option];
  }

  private convertDataToChartData(src: ValueMeasure[]) {
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
}
