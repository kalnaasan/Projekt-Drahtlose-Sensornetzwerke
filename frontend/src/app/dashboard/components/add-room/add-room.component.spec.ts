import { ComponentFixture, TestBed } from '@angular/core/testing';

import { AddRoomComponent } from './add-room.component';

describe('AddRoomDialoggComponent', () => {
  let component: AddRoomComponent;
  let fixture: ComponentFixture<AddRoomComponent>;

  beforeEach(() => {
    TestBed.configureTestingModule({
      declarations: [AddRoomComponent]
    });
    fixture = TestBed.createComponent(AddRoomComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
