import {Component, OnInit} from '@angular/core';
import {Sensor} from "../../common/sensor";
import {Room} from "../../common/room";
import {FormBuilder, FormGroup, Validators} from "@angular/forms";
import {SensorService} from "../../../shared/services/sensor-service";
import {RoomService} from "../../../shared/services/RoomService";
import {ActivatedRoute} from "@angular/router";

@Component({
  selector: 'app-update-room',
  templateUrl: './update-room.component.html',
  styleUrls: ['./update-room.component.scss']
})
export class UpdateRoomComponent implements OnInit {


  public roomForm!: FormGroup;
  public sensors: Sensor[] = [];
  public sensorDropdownSettings: any = {};
  public room!: Room;

  public isDialogVisible: boolean = true;

  constructor(private sensorService: SensorService,
              private formBuilder: FormBuilder,
              private roomService: RoomService,
              private route: ActivatedRoute,
  ) {
    this.route.params.subscribe(params => {
      this.roomService.getRoomById(params['id']).subscribe({
        next: (res: any) => {
          this.room = res.data;
          this.roomForm = this.formBuilder.group({
            sensor: [this.room.sensors, [Validators.required]],
            roomName: [this.room.name, [Validators.required]]
          });
          this.loadAvailableSensors();
        },
        error: (err: any) => console.log(err)
      })
    });


    this.sensorDropdownSettings = {
      singleSelection: false,
      idField: 'sensorId', // Ändere dies entsprechend der tatsächlichen Eigenschaft in deinem Sensor-Objekt
      textField: 'sensorName', // Ändere dies entsprechend der tatsächlichen Eigenschaft in deinem Sensor-Objekt
      selectAllText: 'Alle auswählen',
      unSelectAllText: 'Alle abwählen',
      itemsShowLimit: 3,
      allowSearchFilter: true,
    };
  }

  ngOnInit(): void {

  }

  loadAvailableSensors(): void {
    this.sensorService.getAllSensors().subscribe({
        next: (res: any) => {
          this.sensors = res.data;
          this.sensors = this.sensors.concat(this.room.sensors);
        },
        error: (err: any) => console.log(err.error)
      }
    );
  }

  compareClients(sensor1: Sensor, sensor2: Sensor): boolean {
    return sensor1 && sensor2 ? sensor1.id === sensor2.id : sensor1 === sensor2;
  }

  cancel(): void {
    document.location.href = '/dashboard/dataView';
  }

  onSubmit(): void {
    if (this.roomForm.valid) {
      const roomName = this.roomForm.get('roomName')?.value;
      const selectedSensors: Sensor[] = this.roomForm.get('sensor')?.value;

      console.log('Room Name:', roomName);
      console.log('Selected Sensors:', selectedSensors);

      const room: Room = {
        id: this.room.id,
        name: roomName,
        sensors: selectedSensors
      }
      this.roomService.updateRoom(this.room.id, room).subscribe({
        next: (res: any) => {
          console.log(res.message);
          this.roomForm.get('roomName')?.setValue("");
          this.roomForm.get('sensor')?.setValue([]);

           document.location.href = '/rooms/' + this.room.id;
        },
        error: (err: any) => console.log(err.error)}
      );// Close the dialog after successful submission

      this.isDialogVisible = false;
    }
  }
}
