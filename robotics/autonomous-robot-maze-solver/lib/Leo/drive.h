#ifndef DRIVE_H_
#define DRIVE_H_

class Drive {
    public:
        /**
         * move forward by 
         */
        bool move(int cells);

        /**
         * Steer left/right by 90degrees (pi/2 radians)
         */
        bool steer(bool left=true);

        /**
         * Specify angle in radians
         * #need to confirm orientation
         */
        bool steerAngle(double radians); 
        
    private:
        
}; 

#endif // DRIVE_H_