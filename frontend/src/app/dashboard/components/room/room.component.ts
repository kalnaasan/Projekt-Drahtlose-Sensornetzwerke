import { Component } from '@angular/core';
import {ActivatedRoute} from "@angular/router";
import {ApiService} from "../../../shared/services/api.service";
import {Sensor} from "../../../shared/model/sensor";

@Component({
  selector: 'app-room',
  templateUrl: './room.component.html',
  styleUrls: ['./room.component.scss']
})
export class RoomComponent {

  public sensors: Sensor[] = [];

  constructor(private route: ActivatedRoute,
              private apiService: ApiService) {
    this.route.params.subscribe(params => {
      this.apiService.getSensorsByRoom(params['id']).subscribe({
        next: (res: any) => this.sensors = res.data,
        error: (err: any) => console.log(err)
      })
    });
  }
}
