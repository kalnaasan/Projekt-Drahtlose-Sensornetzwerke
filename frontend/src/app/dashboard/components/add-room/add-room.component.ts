import { Component, OnInit } from '@angular/core';
import {FormBuilder, FormGroup, Validators} from '@angular/forms';
import {SensorService} from "../../../shared/services/sensor-service";
import {Sensor} from "../../common/sensor";
import {Room} from "../../common/room";
import {RoomService} from "../../../shared/services/RoomService";

@Component({
  selector: 'app-add-room-dialogg',
  templateUrl: './add-room.component.html',
  styleUrls: ['./add-room.component.scss']
})
export class AddRoomComponent implements OnInit {
  public roomForm!: FormGroup;
  public sensors: Sensor[] = [];
  public selectedSensors: Sensor[] = [];
  public sensorDropdownSettings: any = {};

  public isDialogVisible: boolean = true;

  constructor(private sensorService: SensorService,
              private formBuilder: FormBuilder,
              private roomService: RoomService
  ) {

    this.roomForm=this.formBuilder.group({
      sensor:['', [Validators.required]],
      roomName:['', [Validators.required]]
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
    this.loadAvailableSensors();
  }

  loadAvailableSensors(): void {
    this.sensorService.getAllSensors().subscribe(
      (res: any) => {
        this.sensors = res.data;
        console.log(this.sensors);
      },
      (err: any) => console.log(err.error)
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
      const selectedSensors:Sensor[] = this.roomForm.get('sensor')?.value;

      console.log('Room Name:', roomName);
      console.log('Selected Sensors:', selectedSensors);

      const room: Room = {
        id: '',
        name: roomName,
        sensors: selectedSensors
      }
      this.roomService.createRoom(room).subscribe(
        (res: any) => {
          console.log(res.message);
          this.roomForm.get('roomName')?.setValue("");
          this.roomForm.get('sensor')?.setValue([]);

          document.location.href='/home';
        },
        (err: any) => console.log(err.error)
      );// Close the dialog after successful submission

      this.isDialogVisible = false;
    }
  }

}
