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
    this.apiService.getValueBySensorTypeAndDate(this.nameChart, '2024-01-08', '2024-01-08').subscribe({
      next: (res: any) => {
        this.data = this.convertDataToChartData(res.data);
        console.log(this.data);
        this.xaxis = this.updateOptionsData['today'];
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

  private getStartCurrentMonth() {
    // Erstelle ein Date-Objekt für den aktuellen Zeitpunkt
    const currentDate = new Date();

    // Hole den Monat (Monate starten bei 0, also addiere 1)
    const currentMonth = currentDate.getMonth() + 1;
    const currentYear = currentDate.getFullYear();

    return '01.' + currentMonth + '.' + currentYear;
  }

  private getLastDayOfMonth() {
    // Erstelle ein Date-Objekt für den ersten Tag des nächsten Monats
    const nextMonth = new Date(new Date().getFullYear(), new Date().getMonth() + 1, 1);

    // Subtrahiere einen Tag, um zum letzten Tag des aktuellen Monats zu gelangen
    const lastDay = new Date(nextMonth.getTime() - 86400000);

    // Formatiere den letzten Tag im gewünschten Format
    return `${lastDay.getDate()}.${lastDay.getMonth() + 1}.${lastDay.getFullYear()}`;
  }

  private convertDataToChartData(src: ValueMeasure[]) {
    return src.map((valueMeasure: ValueMeasure) => [Date.parse(valueMeasure.readAt), valueMeasure.value]);
    // return src.map(({readAt, value}) => [Date.parse(readAt), value]);
  }
}
