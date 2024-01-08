import { Component } from '@angular/core';

@Component({
  selector: 'app-data-view',
  templateUrl: './data-view.component.html',
  styleUrls: ['./data-view.component.scss']
})
export class DataViewComponent {

  public chart: any = [];
  public dataset =[2.33,2.33,2.33,2.33,2.33,2,4.55,4.55,4.55,4.55];
  private data: any;


  onWeek() {
    this.removeDataChart();

    const labels = ["KW1", "KW2", "KW3", "KW4", "KW5", "KW6", "KW7", "KW8", "KW9", "KW10", "KW11", "KW12",
      "KW13", "KW14", "KW15", "KW16", "KW17", "KW18", "KW19", "KW20", "KW21", "KW22", "KW23",
      "KW24", "KW25", "KW26", "KW27", "KW28", "KW29", "KW30", "KW31", "KW32", "KW34", "KW35",
      "KW36", "KW37", "KW38", "KW39", "KW40", "KW41", "KW42", "KW43", "KW44", "KW45", "KW46",
      "KW47", "KW48", "KW49", "KW50", "KW51", "KW52"];

    for (let label of labels) {
      this.chart.data.labels.push(label);
    }


    const dataset = {
      label: '# of Reservations',
      data: this.data,
      backgroundColor: [
        'rgba(255, 99, 132, 0.2)',
        'rgba(54, 162, 235, 0.2)'
      ],
      borderColor: [
        'rgba(255, 99, 132, 1)',
        'rgba(54, 162, 235, 1)',
      ],
      borderWidth: 1
    }

    this.chart.data.datasets.push(dataset);
    this.chart.update();
  }


  onMonth() {
    this.removeDataChart();

    const labels = ['Jan.', 'Feb.', 'Mar.', 'Apr.', 'May', 'June', 'July', 'Aug.', 'Sept.', 'Oct.', 'Nov.', 'Dec.']
    for (let label of labels) {
      this.chart.data.labels.push(label);
    }


        const dataset = {
          label: '# of Reservations',
          data: this.data,      //Array value von mir
          backgroundColor: [
            'rgba(255, 99, 132, 0.2)',
            'rgba(54, 162, 235, 0.2)'
          ],
          borderColor: [
            'rgba(255, 99, 132, 1)',
            'rgba(54, 162, 235, 1)',
          ],
          borderWidth: 1
        }

        this.chart.data.datasets.push(dataset);
        this.chart.update();

  }


  private removeDataChart() {
    this.chart.data.labels = [];
    this.chart.data.datasets.pop();
    this.chart.update();
  }
}
