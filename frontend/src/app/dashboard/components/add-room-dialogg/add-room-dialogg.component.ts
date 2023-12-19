import { Component, OnInit } from '@angular/core';
import {FormBuilder, FormGroup, Validators} from '@angular/forms';
import {SensorService} from "../../service/sensor-service";
import {Sensor} from "../../common/sensor";
import {MatDialog, MatDialogRef} from "@angular/material/dialog";

@Component({
  selector: 'app-add-room-dialogg',
  templateUrl: './add-room-dialogg.component.html',
  styleUrls: ['./add-room-dialogg.component.scss']
})
export class AddRoomDialoggComponent implements OnInit {
  public roomForm!: FormGroup;
  public sensors: Sensor[] = [];
  public selectedSensors: Sensor[] = [];
  public sensorDropdownSettings: any = {};

  public isDialogVisible: boolean = true;


  constructor(private sensorService: SensorService,
              private formBuilder: FormBuilder,
              private dialog: MatDialog
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
    // Set the visibility to false to hide the dialog
    const dialogRef = this.dialog.open(AddRoomDialoggComponent);
    dialogRef.close();
  }

  onSubmit(): void {
    if (this.roomForm.valid) {
      // Retrieve the form values
      const roomName = this.roomForm.get('roomName')?.value;
      const selectedSensors = this.roomForm.get('sensor')?.value;

      // Perform actions related to form submission (e.g., sending data to the server)
      console.log('Room Name:', roomName);
      console.log('Selected Sensors:', selectedSensors);

      // Close the dialog after successful submission
      this.isDialogVisible = false;
    }
  }
}
