import {Component, OnInit} from '@angular/core';
import {RoomService} from "../../../shared/services/RoomService";
import {Room} from "../../common/room";

enum Status {
  Normal = 'success',
  Warning = 'warning',
  Alert = 'danger'
}

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit {
  public rooms: Room[] = [];
  public currenValues: any[] = [];
  private interval!: number;

  constructor(private roomService: RoomService) {
  }

  ngOnInit(): void {
    this.roomService.getAllRooms().subscribe({
      next: (res: any) => this.getSensorValues(res.data),
      error: (err: any) => console.log(err)
    });
    this.interval = setInterval(() => {
      const path = document.location.pathname;
      if (path !== '/home') {
        clearInterval(this.interval);
      } else if (this.rooms.length > 0) {
        this.getSensorValues(this.rooms);
      }
    }, 5000);
  }


  private getSensorValues(data: any[]) {
    this.rooms = data;
    this.rooms.sort((a, b) => {
      if (a.name < b.name) {
        return -1;
      } else if (a.name > b.name) {
        return 1;
      } else {
        return 0;
      }
    });
    let sensorsValues: any[] = [];
    for (const item of data) {
      this.roomService.getSensorValues(item.id).subscribe({
        // next: (res: any) => sensorsValues.push(this.convertDataToChartData(res.data)),
        next: (res: any) => this.updateSensorsValues(sensorsValues, res.data),
        error: (err: any) => console.log(err)
      });
    }
  }

  private updateSensorsValues(sensorsValues: any[], data: any[]) {
    sensorsValues.push(this.convertDataToChartData(data));
    this.currenValues = sensorsValues;
    this.currenValues.sort((a, b) => {
      if (a.roomName < b.roomName) {
        return -1;
      } else if (a.roomName > b.roomName) {
        return 1;
      } else {
        return 0;
      }
    });
  }

  private convertDataToChartData(src: any) {
    for (let i = 0; i < src.valueMeasures.length; i++) {
      if (src.valueMeasures[i].type.toLowerCase().includes('temp') || src.valueMeasures[i].type.toLowerCase().includes('hum')) {
        src.valueMeasures[i].value = Math.round(src.valueMeasures[i].value / 1000);
      } /*else if (src.valueMeasures[i].type.toLowerCase().includes('voc')) {
        src.valueMeasures[i].value = Math.round(src.valueMeasures[i].value / 10);
      }*/
      src.valueMeasures[i].value = Math.round(src.valueMeasures[i].value);
    }
    return src;
  }

  getStatus(item: any) {
    if (item) {
      let status: any[] = []
      item.valueMeasures.forEach((measure: any) => {
        const current = this.checkStatus(measure.value, measure.type);
        status.push(current)
      });
      let currentStatus = status.filter((item: any) => item === Status.Alert)[0];
      if (!currentStatus) {
        currentStatus = status.filter((item: any) => item === Status.Warning)[0];
      }
      if (!currentStatus) {
        currentStatus = status.filter((item: any) => item === Status.Normal)[0];
      }
      return currentStatus;
    }
  }

  checkStatus(value: number, type: string): Status {
    switch (type) {
      case 'TEMP':
        if (value >= 18 && value <= 24) {
          return Status.Normal;
        } else if (value < 16 || value > 26) {
          return Status.Alert;
        } else {
          return Status.Warning
        }
      case 'HUM':
        if (value >= 40 && value <= 60) {
          return Status.Normal;
        } else if (value < 30 || value > 70) {
          return Status.Alert;
        } else {
          return Status.Warning
        }
      case 'VOC':
        if (value >= 0 && value <= 1000) {
          return Status.Normal;
        } else if (value < 0 || value > 1500) {
          return Status.Alert;
        } else {
          return Status.Warning
        }
      case 'CO2':
        if (value >= 0 && value <= 1000) {
          return Status.Normal;
        } else if (value > 2000) {
          return Status.Alert;
        } else {
          return Status.Warning
        }
      default:
        return Status.Warning;
    }
  }
}

