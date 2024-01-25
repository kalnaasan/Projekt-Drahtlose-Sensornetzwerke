import {Component} from '@angular/core';
import {ActivatedRoute} from "@angular/router";
import {ApiService} from "../../../shared/services/api.service";
import {Sensor} from "../../../shared/model/sensor";
import {MatDialog} from "@angular/material/dialog";
import {StatusDialogComponent} from "../status-dialog/status-dialog.component";

@Component({
  selector: 'app-room',
  templateUrl: './room.component.html',
  styleUrls: ['./room.component.scss']
})
export class RoomComponent {

  public sensors: Sensor[] = [];
  public status: any[] = []
  public resolveStatus: boolean = false;

  constructor(private route: ActivatedRoute,
              private apiService: ApiService,
              public dialog: MatDialog) {
    this.route.params.subscribe(params => {
      this.apiService.getSensorsByRoom(params['id']).subscribe({
        next: (res: any) => this.sensors = res.data,
        error: (err: any) => console.log(err)
      })
    });
  }

  addMessage(newItem: any) {
    this.status.push(newItem);
    if (this.status.length === this.sensors.length) {
      console.log(this.status);
      this.openDialog();
    }
  }

  openDialog(): void {
    const dialogRef = this.dialog.open(StatusDialogComponent, {
      data: this.status,
    });

    dialogRef.afterClosed().subscribe(result => {
      this.status = [];
      console.log('The dialog was closed', this.status);
    });
  }
}
