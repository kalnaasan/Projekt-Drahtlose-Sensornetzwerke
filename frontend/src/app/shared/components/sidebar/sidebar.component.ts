// sidebar.component.ts
import { Component, Input, OnInit } from '@angular/core';
import { RouteItem } from "../../common/route-item";
import { Router } from "@angular/router";
import {RoomService} from "../../../dashboard/service/RoomService";
import {Sensor} from "../../../dashboard/common/sensor";
import {Room} from "../../../dashboard/common/room";

@Component({
  selector: 'app-sidebar',
  templateUrl: './sidebar.component.html',
  styleUrls: ['./sidebar.component.scss']
})
export class SidebarComponent implements OnInit {

  @Input()
  public sideBarOpen = true;
  public routeItems: RouteItem[] = new Array<RouteItem>();
  public rooms: Room[] = [];


  constructor(private router: Router,
              private roomService: RoomService) {
  }

  ngOnInit(): void {
    this.roomService.getAllRooms().subscribe(
      (res: any) => {
        this.rooms = res.data;
        console.log(this.rooms);
      },
      (err: any) => console.log(err.error)
    );
  }

  goToSensor() {
    // Use the Router to navigate to the 'shared/sensors' route
    this.router.navigate(['dashboard/sensors']);
  }

  addNewRaum() {
    this.router.navigate(['dashboard/rooms1']);
  }


}
